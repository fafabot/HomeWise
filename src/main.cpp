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

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(100);

    Serial.println();
    Serial.println("==============================================================");
    Serial.println("           HOMEWISE - MONITORAMENTO DE VAZAO DE AGUA          ");
    Serial.println("        Placa: LOLIN NodeMCU V3 (ESP8266) | Pino: D5          ");
    Serial.println("==============================================================");
    Serial.print  (">> FATOR DE CALIBRACAO ATIVO: ");
    Serial.print  (WATER_PULSES_PER_LITER, 1);
    Serial.println(" pulsos/Litro");
    Serial.println(">> INSTRUCOES:");
    Serial.println("   1. O contador inicia zerado.");
    Serial.println("   2. Despeje os 400 ml de agua no funil.");
    Serial.println("   3. As leituras serao exibidas em tempo real.");
    Serial.println("   4. Quando a agua parar, o relatorio final sera impresso.");
    Serial.println("   5. Digite 'r' e aperte Enter para ZERAR a qualquer momento.");
    Serial.println("==============================================================");
    Serial.println(">> Aguardando fluxo de agua...\n");

    iniciarAgua();
    iniciarEnergia();
    iniciarTempo();
    iniciarHistorico();
    iniciarComunicacao();

    ultimosPulsos = 0;
    momentoUltimoPulso = millis();
    emDespejo = false;
    relatorioExibido = false;
}

void loop() {
    atualizarAgua();
    atualizarEnergia();
    atualizarComunicacao();
    enviarDadosAPI();

    const unsigned long pulsosAtuais = obterPulsos();
    const unsigned long agora = millis();

    // 1. Comando Serial para zerar
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == 'r' || c == 'R') {
            resetarConsumoDiarioAgua();
            ultimosPulsos = 0;
            momentoUltimoPulso = agora;
            emDespejo = false;
            relatorioExibido = false;
            Serial.println();
            Serial.println("--------------------------------------------------------------");
            Serial.println(">> [ZERADO] Contador resetado para 0! Pronto para novo teste.");
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
            Serial.println(">> [INICIO] Fluxo detectado! Despejando...");
        }

        // Imprime o progresso a cada 400ms durante o despejo
        if (agora - ultimoPrintFluxo >= 400) {
            ultimoPrintFluxo = agora;
            const float ml = obterLitros() * 1000.0f;
            Serial.print("   -> Pulsos: ");
            if (pulsosAtuais < 100) Serial.print(" ");
            Serial.print(pulsosAtuais);
            Serial.print("  |  Volume: ");
            Serial.print(ml, 1);
            Serial.print(" mL  |  Vazao: ");
            Serial.print(obterVazao(), 2);
            Serial.println(" L/min");
        }
    }

    // 3. Deteccao de fim de fluxo (agua parou por 2 segundos apos o despejo)
    if (emDespejo && !relatorioExibido && (agora - momentoUltimoPulso >= 2000)) {
        emDespejo = false;
        relatorioExibido = true;

        const float litros = obterLitros();
        const float ml = litros * 1000.0f;

        Serial.println();
        Serial.println("==============================================================");
        Serial.println("                  RELATORIO FINAL DA MEDICAO                  ");
        Serial.println("==============================================================");
        Serial.print  ("   Total de Pulsos : ");
        Serial.print  (pulsosAtuais);
        Serial.println(" pulsos");
        Serial.print  ("   Volume Medido   : ");
        Serial.print  (ml, 1);
        Serial.print  (" mL  (");
        Serial.print  (litros, 3);
        Serial.println(" L)");
        Serial.print  ("   Fator Utilizado : ");
        Serial.print  (WATER_PULSES_PER_LITER, 1);
        Serial.println(" pulsos/L");
        Serial.println("==============================================================");
        Serial.println(">> Teste concluido! Envie 'r' para zerar e testar novamente.\n");
    }

    delay(10);
}
