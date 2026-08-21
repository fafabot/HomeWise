#ifndef CONFIG_H
#define CONFIG_H

// ==============================
// HOMEWISE - CONFIGURAÇÕES
// ==============================

// ---------- Sensor de água ----------
#define WATER_SENSOR_PIN 18

// Na simulação, cada clique no botão representa 10 pulsos do sensor.
// Quando o sensor físico for utilizado, altere este valor para 1.
#define WATER_PULSES_PER_EVENT 10

// ---------- Energia / PZEM ----------
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17

// ---------- OLED ----------
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// ---------- Serial ----------
#define SERIAL_BAUD 115200

#endif