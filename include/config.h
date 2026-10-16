#ifndef HOMEWISE_CONFIG_H
#define HOMEWISE_CONFIG_H

#include <Arduino.h>

// Controlador: WeMos D1 R1 (ESP8266)

// Identificador enviado ao Firebase Firestore.
constexpr char DISPOSITIVO_ID[] = "central_homewise_01";

// Configuracao do Google Firebase Firestore (Nuvem)
// Conexao HTTPS direta com a REST API da Google Cloud.
// Isso elimina a necessidade de ter um computador ligado na rede local.
constexpr char FIREBASE_PROJECT_ID[] = "homewise-65bb1";
// A chave FIREBASE_API_KEY fica isolada em include/credenciais.h (protegida pelo .gitignore)

// Sensor de vazao YF-S201 (Calibrado experimentalmente: 161 pulsos / 0.4L = 402.5)
// Na WeMos D1 R1: pino identificado como D5 (ou D13/SCK) = GPIO14
constexpr uint8_t WATER_SENSOR_PIN = D5;  // GPIO14
constexpr unsigned long WATER_PULSES_PER_EVENT = 1UL;
constexpr float WATER_PULSES_PER_LITER = 402.5f;
constexpr unsigned long WATER_FLOW_READ_INTERVAL_MS = 1000UL;

// PZEM-004T v3 usando SoftwareSerial.
// Na WeMos D1 R1: D6 (ou D12/MISO) = GPIO12 | D7 (ou D11/MOSI) = GPIO13
// TX do PZEM -> D6/RX do ESP8266; RX do PZEM <- D7/TX do ESP8266.
constexpr uint8_t PZEM_RX_PIN = D6;  // GPIO12
constexpr uint8_t PZEM_TX_PIN = D7;  // GPIO13

// OLED SSD1306 I2C
// Na WeMos D1 R1: SDA = GPIO4 (pino D4 ou D14/SDA) e SCL = GPIO5 (pino D3 ou D15/SCL)
constexpr uint8_t OLED_SDA_PIN = SDA;  // GPIO4 (D4 / D14)
constexpr uint8_t OLED_SCL_PIN = SCL;  // GPIO5 (D3 / D15)
constexpr int OLED_WIDTH = 128;
constexpr int OLED_HEIGHT = 64;
constexpr int OLED_RESET_PIN = -1;
constexpr uint8_t OLED_I2C_ADDRESS = 0x3C;

// Historico e analise
constexpr int HISTORY_MAX_DAYS = 31;
constexpr int SIMULATED_DAYS_IN_MONTH = 30;
// Em teste fisico, mantemos o dia longo para NAO zerar o contador no meio do despejo
constexpr unsigned long SIMULATED_DAY_DURATION_MS = 86400000UL; // 24 horas
constexpr float ABNORMAL_CONSUMPTION_FACTOR = 1.20f;

// Temporizacao
constexpr unsigned long SERIAL_BAUD = 115200UL;
constexpr unsigned long ENERGY_READ_INTERVAL_MS = 1000UL;
constexpr unsigned long REPORT_INTERVAL_MS = 800UL;
constexpr unsigned long DISPLAY_INTERVAL_MS = 300UL;
// Envio periodico a cada 10s: max 8.640 escritas/dia (plano Spark do Firebase permite 20.000)
constexpr unsigned long API_SEND_INTERVAL_MS = 10000UL;

#endif
