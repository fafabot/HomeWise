#include <Arduino.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>

#include "config.h"
#include "energia.h"

SoftwareSerial pzemSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSerial);

static float tensao = 0.0f;
static float corrente = 0.0f;
static float potencia = 0.0f;
static float energia = 0.0f;

static float energiaTotalKWh = 0.0f;
static float energiaBaseKWh = 0.0f;
static bool baseEnergiaInicializada = false;
static unsigned long ultimaLeitura = 0;

void iniciarEnergia() {
    tensao = 0.0f;
    corrente = 0.0f;
    potencia = 0.0f;
    energia = 0.0f;
    energiaTotalKWh = 0.0f;
    energiaBaseKWh = 0.0f;
    baseEnergiaInicializada = false;
    ultimaLeitura = 0;
}

void atualizarEnergia() {
    unsigned long agora = millis();

    if (agora - ultimaLeitura < ENERGY_READ_INTERVAL_MS) {
        return;
    }

    ultimaLeitura = agora;

    float novaTensao = pzem.voltage();
    float novaCorrente = pzem.current();
    float novaPotencia = pzem.power();
    float novaEnergiaTotal = pzem.energy();

    if (isnan(novaTensao) || isnan(novaCorrente) ||
        isnan(novaPotencia) || isnan(novaEnergiaTotal)) {
        tensao = 0.0f;
        corrente = 0.0f;
        potencia = 0.0f;
        return;
    }

    tensao = novaTensao;
    corrente = novaCorrente;
    potencia = novaPotencia;
    energiaTotalKWh = novaEnergiaTotal;

    if (!baseEnergiaInicializada) {
        energiaBaseKWh = energiaTotalKWh;
        baseEnergiaInicializada = true;
    }

    energia = energiaTotalKWh - energiaBaseKWh;

    if (energia < 0.0f) {
        energiaBaseKWh = energiaTotalKWh;
        energia = 0.0f;
    }
}

float obterTensao() { return tensao; }
float obterCorrente() { return corrente; }
float obterPotencia() { return potencia; }
float obterEnergia() { return energia; }
bool cargaLigada() { return potencia > 1.0f; }

void resetarConsumoDiarioEnergia() {
    if (baseEnergiaInicializada) {
        energiaBaseKWh = energiaTotalKWh;
    }

    energia = 0.0f;
}

