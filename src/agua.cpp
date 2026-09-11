#include <Arduino.h>

#include "agua.h"
#include "config.h"

volatile unsigned long pulsosAgua = 0;

static float litros = 0.0f;
static float vazao = 0.0f;

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
    // Faz uma copia segura do contador que e alterado pela interrupcao.
    noInterrupts();
    unsigned long pulsos = pulsosAgua;
    interrupts();

    litros = pulsos / WATER_PULSES_PER_LITER;

    // A vazao instantanea sera calculada em uma etapa posterior,
    // usando a quantidade de pulsos em uma janela de tempo.
}

float obterLitros() {
    return litros;
}

float obterVazao() {
    return vazao;
}

unsigned long obterPulsos() {
    noInterrupts();
    unsigned long pulsos = pulsosAgua;
    interrupts();
    return pulsos;
}

void resetarConsumoDiarioAgua() {
    noInterrupts();
    pulsosAgua = 0;
    interrupts();

    litros = 0.0f;
    vazao = 0.0f;
}
