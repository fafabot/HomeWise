#include <Arduino.h>

#include "energia.h"
#include "config.h"

static float tensao = 0.0f;
static float corrente = 0.0f;
static float potencia = 0.0f;
static float energia = 0.0f;
static bool cargaLigadaEstado = false;
static unsigned long ultimaAtualizacao = 0;

void iniciarEnergia() {
    ultimaAtualizacao = millis();
    tensao = 0.0f;
    corrente = 0.0f;
    potencia = 0.0f;
    energia = 0.0f;
    cargaLigadaEstado = false;
}

void definirCarga(bool ligada) {
    // Contabiliza o periodo anterior antes de trocar o estado da carga.
    atualizarEnergia();
    cargaLigadaEstado = ligada;
}

void atualizarEnergia() {
    unsigned long agora = millis();
    float horas = (agora - ultimaAtualizacao) / 3600000.0f;

    if (cargaLigadaEstado) {
        tensao = SIMULATED_VOLTAGE_V;
        potencia = SIMULATED_POWER_W;
        corrente = potencia / tensao;
        energia += (potencia / 1000.0f) * horas;
    } else {
        tensao = 0.0f;
        corrente = 0.0f;
        potencia = 0.0f;
    }

    ultimaAtualizacao = agora;
}

float obterTensao() { return tensao; }
float obterCorrente() { return corrente; }
float obterPotencia() { return potencia; }
float obterEnergia() { return energia; }
bool cargaLigada() { return cargaLigadaEstado; }

void resetarConsumoDiarioEnergia() {
    energia = 0.0f;
    ultimaAtualizacao = millis();
}
