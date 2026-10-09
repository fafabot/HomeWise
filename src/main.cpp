#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Protótipos das funções auxiliares
void inicializarSensorAgua();
float obterVolumeLitros();
struct LeituraEletrica { float tensao; float corrente; float potencia; float energiaAcumulada; };
void inicializarSensorEnergia();
LeituraEletrica obterLeituraEletrica();

unsigned long ultimoEnvio = 0;
const unsigned long INTERVALO_ENVIO_MS = 5000;

void setup() {
    Serial.begin(115200);
    inicializarSensorAgua();
    inicializarSensorEnergia();

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Conectando ao Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi Conectado!");
}

void loop() {
    if (millis() - ultimoEnvio >= INTERVALO_ENVIO_MS) {
        ultimoEnvio = millis();

        if (WiFi.status() == WL_CONNECTED) {
            float volume = obterVolumeLitros();
            LeituraEletrica eletrica = obterLeituraEletrica();

            WiFiClient client;
            HTTPClient http;
            http.begin(client, API_URL);
            http.addHeader("Content-Type", "application/json");

            StaticJsonDocument<256> doc;
            doc["dispositivo_id"] = DISPOSITIVO_ID;
            doc["consumo_agua_litros"] = volume;
            doc["potencia_w"] = eletrica.potencia;
            doc["energia_kwh"] = eletrica.energiaAcumulada;
            doc["tensao_v"] = eletrica.tensao;

            String payload;
            serializeJson(doc, payload);

            int statusHttp = http.POST(payload);
            Serial.printf("Envio realizado. Status HTTP: %d\n", statusHttp);
            http.end();
        } else {
            Serial.println("Wi-Fi desconectado. Aguardando reconexão...");
        }
    }
}

#include "agua.h"
#include "analise.h"
#include "comunicacao.h"
#include "energia.h"
#include "historico.h"
#include "tempo.h"

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET_PIN);

static bool oledInicializado = false;
static unsigned long lastReport = 0;
static unsigned long lastDisplay = 0;

void updateDisplay() {
    if (!oledInicializado || millis() - lastDisplay < DISPLAY_INTERVAL_MS) {
        return;
    }

    lastDisplay = millis();

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.println("HOMEWISE");
    display.println("----------------");
    display.print("Agua: ");
    display.print(obterLitros(), 2);
    display.println(" L");
    display.print("Pulsos: ");
    display.println(obterPulsos());
    display.print("Vazao: ");
    display.print(obterVazao(), 2);
    display.println(" L/min");
    display.print("Energia: ");
    display.print(obterEnergia(), 4);
    display.println(" kWh");
    display.print("Carga: ");
    display.println(cargaLigada() ? "ON" : "OFF");
    display.print("Pot.: ");
    display.print(obterPotencia(), 0);
    display.println(" W");
    display.display();
}

void printReport() {
    Serial.println();
    Serial.println("----------- HOMEWISE -----------");
    Serial.print("Agua: ");
    Serial.print(obterLitros(), 3);
    Serial.println(" L");
    Serial.print("Vazao: ");
    Serial.print(obterVazao(), 2);
    Serial.println(" L/min");
    Serial.print("Tensao: ");
    Serial.print(obterTensao(), 1);
    Serial.println(" V");
    Serial.print("Corrente: ");
    Serial.print(obterCorrente(), 2);
    Serial.println(" A");
    Serial.print("Potencia: ");
    Serial.print(obterPotencia(), 1);
    Serial.println(" W");
    Serial.print("Energia diaria: ");
    Serial.print(obterEnergia(), 4);
    Serial.println(" kWh");
    Serial.println("--------------------------------");
}

void printDailySummary(int diaFinalizado) {
    Serial.println();
    Serial.print("Dia ");
    Serial.print(diaFinalizado);
    Serial.println(" finalizado!");

    Serial.print("Consumo de agua: ");
    Serial.print(obterAguaDoDia(diaFinalizado), 3);
    Serial.println(" L");

    Serial.print("Consumo de energia: ");
    Serial.print(obterEnergiaDoDia(diaFinalizado), 4);
    Serial.println(" kWh");

    if (possuiBaseComparacao(diaFinalizado)) {
        Serial.print("Agua: ");
        Serial.println(consumoAguaAnormal(diaFinalizado) ? "ANORMAL" : "NORMAL");
        Serial.print("Energia: ");
        Serial.println(consumoEnergiaAnormal(diaFinalizado) ? "ANORMAL" : "NORMAL");
    } else {
        Serial.println("Analise: SEM HISTORICO PARA COMPARACAO");
    }

    Serial.print("Media diaria de agua: ");
    Serial.print(calcularMediaAgua(), 3);
    Serial.println(" L");
    Serial.print("Previsao mensal de agua: ");
    Serial.print(calcularPrevisaoMensalAgua(), 3);
    Serial.println(" L");

    Serial.print("Media diaria de energia: ");
    Serial.print(calcularMediaEnergia(), 4);
    Serial.println(" kWh");
    Serial.print("Previsao mensal de energia: ");
    Serial.print(calcularPrevisaoMensalEnergia(), 4);
    Serial.println(" kWh");

    if (diaFinalizado < obterDiasNoMes()) {
        Serial.print("Novo dia: ");
        Serial.println(diaFinalizado + 1);
    } else {
        Serial.println("Historico mensal completo.");
    }
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(50);

    Serial.println();
    Serial.println("================================");
    Serial.println(" HOMEWISE - LOLIN NodeMCU V3");
    Serial.println("================================");
    Serial.println("Iniciando prototipo fisico...");

    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

    oledInicializado = display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);

    if (!oledInicializado) {
        Serial.println("ERRO: OLED nao inicializado.");
    } else {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("HOMEWISE");
        display.println("NodeMCU V3");
        display.println();
        display.println("Agua + Energia");
        display.display();
    }

    iniciarAgua();
    iniciarEnergia();
    iniciarTempo();
    iniciarHistorico();
    iniciarComunicacao();

    Serial.println("Sistema pronto!");
}

void loop() {
    atualizarAgua();
    atualizarEnergia();
    atualizarComunicacao();
    updateDisplay();
    enviarDadosSimulados();

    int diaFinalizado = verificarDia();
    if (diaFinalizado != 0) {
        printDailySummary(diaFinalizado);
    }

    if (millis() - lastReport >= REPORT_INTERVAL_MS) {
        lastReport = millis();
        printReport();
    }

    delay(10);
}

