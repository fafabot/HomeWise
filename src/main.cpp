#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "agua.h"
#include "energia.h"
#include "config.h"
#include "historico.h"

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

unsigned long lastReport = 0;
unsigned long lastDisplay = 0;

void updateDisplay() {
  if (millis() - lastDisplay < DISPLAY_UPDATE_INTERVAL_MS) {
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

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(50);

  Serial.println();
  Serial.println("================================");
  Serial.println(" HOMEWISE - LOLIN NodeMCU V3");
  Serial.println("================================");
  Serial.println("Iniciando prototipo fisico...");

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
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
  iniciarHistorico();

  Serial.println("Sistema pronto!");
}

void loop() {
  atualizarAgua();
  atualizarEnergia();
  updateDisplay();
  verificarDia();

  if (millis() - lastReport >= REPORT_INTERVAL_MS) {
    lastReport = millis();
    printReport();
  }

  delay(10);
}
