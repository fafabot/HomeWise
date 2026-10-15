#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "energia.h"

// RX do ESP8266 recebe o TX do PZEM; TX do ESP8266 envia ao RX do PZEM.
static SoftwareSerial pzemSerial(PZEM_RX_PIN, PZEM_TX_PIN);
static PZEM004Tv30 pzem(pzemSerial);

static float tensao = 0.0f;
static float corrente = 0.0f;
static float potencia = 0.0f;
static float energia = 0.0f;

static float energiaTotalKWh = 0.0f;
static float energiaBaseKWh = 0.0f;
static bool baseEnergiaInicializada = false;
static unsigned long ultimaLeitura = 0;
static bool comunicando = false;

void iniciarEnergia() {
    pzemSerial.begin(9600);

    tensao = 0.0f;
    corrente = 0.0f;
    potencia = 0.0f;
    energia = 0.0f;
    energiaTotalKWh = 0.0f;
    energiaBaseKWh = 0.0f;
    baseEnergiaInicializada = false;
    ultimaLeitura = 0;
    comunicando = false;
}

void atualizarEnergia() {
    const unsigned long agora = millis();
    if (agora - ultimaLeitura < ENERGY_READ_INTERVAL_MS) {
        return;
    }

    ultimaLeitura = agora;

    const float novaTensao = pzem.voltage();
    const float novaCorrente = pzem.current();
    const float novaPotencia = pzem.power();
    const float novaEnergiaTotal = pzem.energy();

    if (isnan(novaTensao)) {
        comunicando = false;
        return;
    }

    comunicando = true;
    tensao = novaTensao;
    corrente = isnan(novaCorrente) ? 0.0f : novaCorrente;
    potencia = isnan(novaPotencia) ? 0.0f : novaPotencia;

    if (!isnan(novaEnergiaTotal)) {
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
}

float obterTensao() { return tensao; }
float obterCorrente() { return corrente; }
float obterPotencia() { return potencia; }
float obterEnergia() { return energia; }
bool cargaLigada() { return potencia > 1.0f; }
bool pzemComunicando() { return comunicando; }

void resetarConsumoDiarioEnergia() {
    if (baseEnergiaInicializada) {
        energiaBaseKWh = energiaTotalKWh;
    }
    energia = 0.0f;
}
