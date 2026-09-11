#ifndef CONFIG_H
#define CONFIG_H

// ==============================
// HOMEWISE - CONFIGURACOES
// Controlador oficial: LOLIN NodeMCU V3 (ESP8266)
// ==============================

// ---------- Sensor de agua ----------
// D5 = GPIO14. Pino utilizado para a interrupcao do sensor de vazao.
#define WATER_SENSOR_PIN D5

// No sensor fisico, cada pulso recebido conta como 1 evento.
#define WATER_PULSES_PER_EVENT 1

// Valor inicial de calibracao do YF-S201.
// Deve ser ajustado posteriormente com testes reais de volume.
#define WATER_PULSES_PER_LITER 450.0f

// ---------- Energia / PZEM-004T v3 ----------
// SoftwareSerial no ESP8266.
// RX do ESP8266 recebe TX do PZEM; TX do ESP8266 envia ao RX do PZEM.
#define PZEM_RX_PIN D6
#define PZEM_TX_PIN D7

// ---------- OLED I2C ----------
// D2 = GPIO4 (SDA) e D1 = GPIO5 (SCL).
#define OLED_SDA_PIN D2
#define OLED_SCL_PIN D1

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDRESS 0x3C

// ---------- Serial USB ----------
#define SERIAL_BAUD 115200

// ---------- Intervalos ----------
#define ENERGY_READ_INTERVAL_MS 1000UL
#define DISPLAY_UPDATE_INTERVAL_MS 300UL
#define REPORT_INTERVAL_MS 2000UL

#endif
