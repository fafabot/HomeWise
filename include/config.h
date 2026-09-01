#ifndef CONFIG_H
#define CONFIG_H

// Sensor de agua e botao de simulacao
constexpr int WATER_SENSOR_PIN = 18;
constexpr unsigned long WATER_PULSES_PER_EVENT = 10UL;
constexpr float WATER_PULSES_PER_LITER = 450.0f;

// Botao da carga de energia simulada
constexpr int ENERGY_BUTTON_PIN = 19;

// Energia / PZEM
constexpr int PZEM_RX_PIN = 16;
constexpr int PZEM_TX_PIN = 17;
constexpr float SIMULATED_VOLTAGE_V = 127.0f;
constexpr float SIMULATED_POWER_W = 500.0f;

// OLED
constexpr int OLED_SDA_PIN = 21;
constexpr int OLED_SCL_PIN = 22;
constexpr int OLED_WIDTH = 128;
constexpr int OLED_HEIGHT = 64;
constexpr int OLED_RESET_PIN = -1;
constexpr int OLED_I2C_ADDRESS = 0x3C;

// Historico e analise
constexpr int HISTORY_MAX_DAYS = 31;
constexpr int SIMULATED_DAYS_IN_MONTH = 30;
constexpr unsigned long SIMULATED_DAY_DURATION_MS = 10000UL;
constexpr float ABNORMAL_CONSUMPTION_FACTOR = 1.20f;

// Temporizacao e Serial
constexpr unsigned long SERIAL_BAUD = 115200UL;
constexpr unsigned long REPORT_INTERVAL_MS = 2000UL;
constexpr unsigned long DISPLAY_INTERVAL_MS = 300UL;

#endif
