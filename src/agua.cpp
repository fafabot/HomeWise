#include <Arduino.h>

#include "agua.h"
#include "config.h"

static volatile unsigned long pulsosAgua = 0;
static float litros = 0.0f;
static float vazao = 0.0f;

static unsigned long copiarPulsosComSeguranca() {
    noInterrupts();
    unsigned long copia = pulsosAgua;
    interrupts();
    return copia;
}

void ICACHE_RAM_ATTR contarPulso() {
    pulsosAgua += WATER_PULSES_PER_EVENT;
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
    litros = copiarPulsosComSeguranca() / WATER_PULSES_PER_LITER;
}

float obterLitros() {
    return litros;
}

float obterVazao() {
    return vazao;
}

unsigned long obterPulsos() {
    return copiarPulsosComSeguranca();
}

void resetarConsumoDiarioAgua() {
    noInterrupts();
    pulsosAgua = 0;
    interrupts();

    litros = 0.0f;
    vazao = 0.0f;
}

