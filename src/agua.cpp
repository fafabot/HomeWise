#include <Arduino.h>

#include "agua.h"
#include "config.h"

static volatile unsigned long pulsosAgua = 0;
static float litros = 0.0f;
static float vazao = 0.0f;

static unsigned long pulsosAnterioresVazao = 0;
static unsigned long momentoUltimaVazao = 0;
static bool vazaoInicializada = false;

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

    litros = 0.0f;
    vazao = 0.0f;
    pulsosAnterioresVazao = 0;
    momentoUltimaVazao = millis();
    vazaoInicializada = false;
}

void atualizarAgua() {
    unsigned long pulsosAtuais = copiarPulsosComSeguranca();

    litros = pulsosAtuais / WATER_PULSES_PER_LITER;

    unsigned long agora = millis();

    if (!vazaoInicializada) {
        pulsosAnterioresVazao = pulsosAtuais;
        momentoUltimaVazao = agora;
        vazaoInicializada = true;
        vazao = 0.0f;
        return;
    }

    unsigned long tempoDecorrido = agora - momentoUltimaVazao;

    if (tempoDecorrido < WATER_FLOW_READ_INTERVAL_MS) {
        return;
    }

    unsigned long pulsosDecorridos = pulsosAtuais - pulsosAnterioresVazao;

    float litrosDecorridos =
        pulsosDecorridos / WATER_PULSES_PER_LITER;

    vazao =
        litrosDecorridos * 60000.0f / tempoDecorrido;

    pulsosAnterioresVazao = pulsosAtuais;
    momentoUltimaVazao = agora;
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

    pulsosAnterioresVazao = 0;
    momentoUltimaVazao = millis();
    vazaoInicializada = false;
}
