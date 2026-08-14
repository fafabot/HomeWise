#include <Arduino.h>

#include "agua.h"
#include "config.h"

volatile unsigned long pulsosAgua = 0;

float litros = 0.0;
float vazao = 0.0;

// Valor inicial para simulação.
// Será calibrado quando o sensor físico for utilizado.
const float PULSOS_POR_LITRO = 450.0;

void IRAM_ATTR contarPulso() {
    pulsosAgua++;
}

void iniciarAgua() {

    pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(WATER_SENSOR_PIN),
        contarPulso,
        FALLING
    );
}

void atualizarAgua() {

    litros = pulsosAgua / PULSOS_POR_LITRO;

    // A vazão será aprimorada posteriormente
    // utilizando uma janela de tempo.
}

float obterLitros() {
    return litros;
}

float obterVazao() {
    return vazao;
}

unsigned long obterPulsos() {
    return pulsosAgua;
}