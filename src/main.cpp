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

const float PULSES_PER_LITER = 450.0;
const int PULSES_PER_PRESS = 10;
const float SIM_VOLTAGE = 127.0;
const float SIM_POWER_W = 500.0;

volatile unsigned long totalWaterPulses = 0;
float totalWaterLiters = 0.0;

bool energyLoadOn = false;
float totalEnergyKWh = 0.0;

bool lastWaterButton = HIGH;
bool lastEnergyButton = HIGH;

unsigned long lastEnergyUpdate = 0;
unsigned long lastReport = 0;
unsigned long lastDisplay = 0;

void handleWaterButton() {
  bool current = digitalRead(WATER_BUTTON);

  if (lastWaterButton == HIGH && current == LOW) {
    totalWaterPulses += PULSES_PER_PRESS;
    totalWaterLiters = totalWaterPulses / PULSES_PER_LITER;

    Serial.print("[AGUA] Pulsos: ");
    Serial.print(totalWaterPulses);
    Serial.print(" | Consumo: ");
    Serial.print(totalWaterLiters, 3);
    Serial.println(" L");
  }

  lastWaterButton = current;
}

void handleEnergyButton() {
  bool current = digitalRead(ENERGY_BUTTON);

  if (lastEnergyButton == HIGH && current == LOW) {
    energyLoadOn = !energyLoadOn;

    Serial.print("[ENERGIA] Liquidificador: ");
    Serial.println(energyLoadOn ? "LIGADO" : "DESLIGADO");
  }

  lastEnergyButton = current;
}

void updateEnergy() {
  unsigned long now = millis();
  float elapsedHours = (now - lastEnergyUpdate) / 3600000.0;

  if (energyLoadOn) {
    totalEnergyKWh += (SIM_POWER_W / 1000.0) * elapsedHours;
  }

  lastEnergyUpdate = now;
}

void updateDisplay() {
  if (millis() - lastDisplay < 300) return;
  lastDisplay = millis();

  float currentA = energyLoadOn ? SIM_POWER_W / SIM_VOLTAGE : 0.0;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  display.println("HOMEWISE");
  display.println("----------------");
  display.print("Agua: ");
  display.print(totalWaterLiters, 2);
  display.println(" L");

  display.print("Pulsos: ");
  display.println(totalWaterPulses);

  display.print("Energia: ");
  display.print(totalEnergyKWh, 4);
  display.println(" kWh");

  display.print("Carga: ");
  display.println(energyLoadOn ? "ON" : "OFF");

  display.print("Pot.: ");
  display.print(energyLoadOn ? SIM_POWER_W : 0);
  display.println(" W");

  display.display();
}

void printReport() {
  float currentA = energyLoadOn ? SIM_POWER_W / SIM_VOLTAGE : 0.0;

  Serial.println();
  Serial.println("----------- HOMEWISE -----------");

  Serial.print("Agua: ");
  Serial.print(totalWaterLiters, 3);
  Serial.println(" L");

  Serial.print("Tensao: ");
  Serial.print(energyLoadOn ? SIM_VOLTAGE : 0.0, 1);
  Serial.println(" V");

  Serial.print("Corrente: ");
  Serial.print(currentA, 2);
  Serial.println(" A");

  Serial.print("Potencia: ");
  Serial.print(energyLoadOn ? SIM_POWER_W : 0.0, 1);
  Serial.println(" W");

  Serial.print("Energia acumulada: ");
  Serial.print(totalEnergyKWh, 4);
  Serial.println(" kWh");

  Serial.println("--------------------------------");
}

void setup() {
  Serial.begin(115200);

  Serial.println("================================");
  Serial.println("       HOMEWISE - ESP32");
  Serial.println("================================");
  Serial.println("Sistema de energia iniciado.");
  Serial.println("Sistema iniciado.");

  pinMode(WATER_BUTTON, INPUT_PULLUP);
  pinMode(ENERGY_BUTTON, INPUT_PULLUP);

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED nao inicializado.");
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("HOMEWISE");
  display.println("Simulacao Wokwi");
  display.println();
  display.println("Agua + Energia");
  display.display();

  lastEnergyUpdate = millis();

  iniciarEnergia();
  iniciarAgua();
}

void loop() {
  handleWaterButton();
  handleEnergyButton();
  updateEnergy();
  updateDisplay();

  if (millis() - lastReport >= 2000) {
    lastReport = millis();
    printReport();
  }

  delay(10);
}