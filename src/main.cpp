#include <Arduino.h>

#include "config.h"
#include "agua.h"
#include "analise.h"
#include "comunicacao.h"
#include "energia.h"
#include "historico.h"
#include "tempo.h"

static unsigned long ultimosPulsos = 0;
static unsigned long momentoUltimoPulso = 0;
static bool emDespejo = false;
static bool relatorioExibido = false;
static unsigned long ultimoPrintFluxo = 0;
static unsigned long ultimoPrintEnergia = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(100);

    Serial.println();
    Serial.println("==============================================================");
    Serial.println("          HOMEWISE - MONITORAMENTO DE AGUA E ENERGIA          ");
    Serial.println("      WeMos D1 R1 (ESP8266) | Agua: D5 | PZEM: D6/D7 (UART)   ");
    Serial.println("==============================================================");
    Serial.println(">> SENSORES CONFIGURADOS:");
    Serial.print  ("   - Sensor YF-S201 (Agua): Pino D5 (GPIO14) | Calibracao: ");
    Serial.print  (WATER_PULSES_PER_LITER, 1);
    Serial.println(" pulsos/L");
    Serial.println("   - Medidor PZEM-004T (Energia): D6/GPIO12 (RX) e D7/GPIO13 (TX)");
    Serial.println(">> COMANDOS SERIAL:");
    Serial.println("   - 'r' + Enter : Zera contador de agua e energia.");
    Serial.println("   - 'p' + Enter : Imprime medicao imediata do PZEM.");
    Serial.println("==============================================================");
    Serial.println(">> Sistema iniciado. Monitorando...\n");

    iniciarAgua();
    iniciarEnergia();
    iniciarTempo();
    iniciarHistorico();
    iniciarComunicacao();

    ultimosPulsos = 0;
    momentoUltimoPulso = millis();
    emDespejo = false;
    relatorioExibido = false;
    ultimoPrintFluxo = 0;
    ultimoPrintEnergia = 0;
}

void loop() {
    atualizarAgua();
    atualizarEnergia();
    atualizarComunicacao();
    enviarDadosAPI();

    const unsigned long pulsosAtuais = obterPulsos();
    const unsigned long agora = millis();

    // 1. Comandos recebidos via Monitor Serial
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == 'r' || c == 'R') {
            resetarConsumoDiarioAgua();
            resetarConsumoDiarioEnergia();
            ultimosPulsos = 0;
            momentoUltimoPulso = agora;
            emDespejo = false;
            relatorioExibido = false;
            Serial.println();
            Serial.println("--------------------------------------------------------------");
            Serial.println(">> [ZERADO] Contador de agua e acumulado de energia resetados!");
            Serial.println("--------------------------------------------------------------\n");
        } else if (c == 'p' || c == 'P') {
            Serial.println();
            Serial.println("--------------------------------------------------------------");
            Serial.println(">> [LEITURA MANUAL PZEM-004T v3]:");
            if (pzemComunicando()) {
                Serial.print("   Tensao   : "); Serial.print(obterTensao(), 1); Serial.println(" V");
                Serial.print("   Corrente : "); Serial.print(obterCorrente(), 3); Serial.println(" A");
                Serial.print("   Potencia : "); Serial.print(obterPotencia(), 1); Serial.println(" W");
                Serial.print("   Energia  : "); Serial.print(obterEnergia(), 4); Serial.println(" kWh");
            } else {
                Serial.println("   Status   : SEM COMUNICACAO COM O PZEM.");
                Serial.println("   Dica     : O PZEM precisa estar conectado na tomada AC 127/220V");
                Serial.println("              para o chip interno ligar e responder pela serial!");
            }
            Serial.println("--------------------------------------------------------------\n");
        }
    }

    // 2. Deteccao de fluxo de agua
    if (pulsosAtuais > ultimosPulsos) {
        ultimosPulsos = pulsosAtuais;
        momentoUltimoPulso = agora;

        if (!emDespejo) {
            emDespejo = true;
            relatorioExibido = false;
            Serial.println(">> [AGUA - INICIO] Fluxo detectado! Despejando...");
        }

        // Imprime o progresso da agua a cada 400ms durante o despejo
        if (agora - ultimoPrintFluxo >= 400) {
            ultimoPrintFluxo = agora;
            const float ml = obterLitros() * 1000.0f;
            Serial.print("   -> [AGUA] Pulsos: ");
            if (pulsosAtuais < 100) Serial.print(" ");
            Serial.print(pulsosAtuais);
            Serial.print("  |  Volume: ");
            Serial.print(ml, 1);
            Serial.print(" mL  |  Vazao: ");
            Serial.print(obterVazao(), 2);
            Serial.println(" L/min");
        }
    }

    // 3. Fim do fluxo de agua (2 segundos sem pulsos apos o despejo)
    if (emDespejo && !relatorioExibido && (agora - momentoUltimoPulso >= 2000)) {
        emDespejo = false;
        relatorioExibido = true;

        const float litros = obterLitros();
        const float ml = litros * 1000.0f;

        Serial.println();
        Serial.println("==============================================================");
        Serial.println("                 RELATORIO CONSOLIDADO DE MEDICAO             ");
        Serial.println("==============================================================");
        Serial.println(">> CONSUMO DE AGUA:");
        Serial.print  ("   Total de Pulsos : ");
        Serial.print  (pulsosAtuais);
        Serial.println(" pulsos");
        Serial.print  ("   Volume Medido   : ");
        Serial.print  (ml, 1);
        Serial.print  (" mL  (");
        Serial.print  (litros, 3);
        Serial.println(" L)");
        Serial.print  ("   Fator Calibrado : ");
        Serial.print  (WATER_PULSES_PER_LITER, 1);
        Serial.println(" pulsos/L");
        Serial.println(">> CONSUMO DE ENERGIA:");
        if (pzemComunicando()) {
            Serial.print("   Tensao da Rede  : "); Serial.print(obterTensao(), 1); Serial.println(" V");
            Serial.print("   Corrente        : "); Serial.print(obterCorrente(), 2); Serial.println(" A");
            Serial.print("   Potencia Ativa  : "); Serial.print(obterPotencia(), 1); Serial.println(" W");
            Serial.print("   Energia         : "); Serial.print(obterEnergia(), 4); Serial.println(" kWh");
        } else {
            Serial.println("   PZEM nao comunicando no momento.");
        }
        Serial.println("==============================================================");
        Serial.println(">> Envie 'r' para zerar tudo ou 'p' para ler a energia.\n");
    }

    // 4. Medicao periodica de energia (a cada 2 segundos quando nao estiver despejando agua)
    if (!emDespejo && (agora - ultimoPrintEnergia >= 2000)) {
        ultimoPrintEnergia = agora;
        if (pzemComunicando()) {
            Serial.print("[ENERGIA] Tensao: ");
            Serial.print(obterTensao(), 1);
            Serial.print(" V  |  Corrente: ");
            Serial.print(obterCorrente(), 2);
            Serial.print(" A  |  Potencia: ");
            Serial.print(obterPotencia(), 1);
            Serial.print(" W  |  Energia: ");
            Serial.print(obterEnergia(), 4);
            Serial.println(" kWh");
        } else {
            Serial.println("[ENERGIA] Aguardando PZEM... (Conecte 5V/VIN, D6/D7 e a tomada AC 127/220V)");
        }
    }

    delay(10);
}
