#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Controlador oficial: LOLIN NodeMCU V3 (ESP8266)

// Sensor de vazao YF-S201
constexpr uint8_t WATER_SENSOR_PIN = D5;  // GPIO14
constexpr unsigned long WATER_PULSES_PER_EVENT = 1UL;
constexpr float WATER_PULSES_PER_LITER = 450.0f;

// PZEM-004T v3 usando SoftwareSerial
// RX do ESP8266 recebe o TX do PZEM; TX do ESP8266 envia ao RX do PZEM.
constexpr uint8_t PZEM_RX_PIN = D6;  // GPIO12
constexpr uint8_t PZEM_TX_PIN = D7;  // GPIO13

// OLED SSD1306 I2C
constexpr uint8_t OLED_SDA_PIN = D2;  // GPIO4
constexpr uint8_t OLED_SCL_PIN = D1;  // GPIO5
constexpr int OLED_WIDTH = 128;
constexpr int OLED_HEIGHT = 64;
constexpr int OLED_RESET_PIN = -1;
constexpr uint8_t OLED_I2C_ADDRESS = 0x3C;

// Historico e analise
constexpr int HISTORY_MAX_DAYS = 31;
constexpr int SIMULATED_DAYS_IN_MONTH = 30;
constexpr unsigned long SIMULATED_DAY_DURATION_MS = 10000UL;
constexpr float ABNORMAL_CONSUMPTION_FACTOR = 1.20f;

// Temporizacao e Serial
constexpr unsigned long SERIAL_BAUD = 115200UL;
constexpr unsigned long ENERGY_READ_INTERVAL_MS = 1000UL;
constexpr unsigned long REPORT_INTERVAL_MS = 2000UL;
constexpr unsigned long DISPLAY_INTERVAL_MS = 300UL;

#endif

