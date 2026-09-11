#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "agua.h"
#include "analise.h"
#include "config.h"
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

    Serial.println("Sistema pronto!");
}

void loop() {
    atualizarAgua();
    atualizarEnergia();
    updateDisplay();

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

