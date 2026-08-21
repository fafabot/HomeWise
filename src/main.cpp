#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "agua.h"
#include "energia.h"
#include "config.h"

#define WATER_BUTTON 18
#define ENERGY_BUTTON 19

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool lastWaterButton = HIGH;
bool lastEnergyButton = HIGH;

unsigned long lastReport = 0;
unsigned long lastDisplay = 0;

void handleWaterButton() {
  bool current = digitalRead(WATER_BUTTON);

  if (lastWaterButton == HIGH && current == LOW) {
    atualizarAgua();

    Serial.print("[AGUA] Pulsos: ");
    Serial.print(obterPulsos());
    Serial.print(" | Consumo: ");
    Serial.print(obterLitros(), 3);
    Serial.println(" L");
  }

  lastWaterButton = current;
}

void handleEnergyButton() {
  bool current = digitalRead(ENERGY_BUTTON);

  if (lastEnergyButton == HIGH && current == LOW) {
    bool novaCarga = !cargaLigada();
    definirCarga(novaCarga);

    Serial.print("[ENERGIA] Liquidificador: ");
    Serial.println(novaCarga ? "LIGADO" : "DESLIGADO");
  }

  lastEnergyButton = current;
}

void updateDisplay() {
  if (millis() - lastDisplay < 300) return;
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

  Serial.print("Energia acumulada: ");
  Serial.print(obterEnergia(), 4);
  Serial.println(" kWh");

  Serial.println("--------------------------------");
}

void setup() {
  Serial.begin(115200);

  Serial.println("================================");
  Serial.println("       HOMEWISE - ESP32");
  Serial.println("================================");
  Serial.println("Iniciando simulacao...");

  pinMode(WATER_BUTTON, INPUT_PULLUP);
  pinMode(ENERGY_BUTTON, INPUT_PULLUP);

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  momentoInicioDia = millis();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERRO: OLED nao inicializado.");
  } else {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("HOMEWISE");
    display.println("Simulacao Wokwi");
    display.println();
    display.println("Agua + Energia");
    display.display();
  }

  iniciarEnergia();
  iniciarAgua();

  Serial.println("Sistema pronto!");
}

void loop() {
  handleWaterButton();
  handleEnergyButton();
  atualizarAgua();
  atualizarEnergia();
  updateDisplay();

  if (millis() - lastReport >= 2000) {
    lastReport = millis();
    printReport();
  }

  delay(10);
}
