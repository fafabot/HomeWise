#include <Arduino.h>

#include "energia.h"
#include "config.h"

// Simulação inicial da carga elétrica.
// Quando o PZEM-004T for instalado, este módulo será substituído
// pelas leituras reais de tensão, corrente, potência e energia.

static float tensao = 0.0;
static float corrente = 0.0;
static float potencia = 0.0;
static float energia = 0.0;
static bool cargaLigadaEstado = false;
static unsigned long ultimaAtualizacao = 0;

static const float SIM_VOLTAGE = 127.0;
static const float SIM_POWER_W = 500.0;

void iniciarEnergia() {
    ultimaAtualizacao = millis();
    tensao = 0.0;
    corrente = 0.0;
    potencia = 0.0;
    energia = 0.0;
    cargaLigadaEstado = false;
}

void definirCarga(bool ligada) {
    cargaLigadaEstado = ligada;
}

void atualizarEnergia() {
    unsigned long agora = millis();
    float horas = (agora - ultimaAtualizacao) / 3600000.0;

    if (cargaLigadaEstado) {
        tensao = SIM_VOLTAGE;
        potencia = SIM_POWER_W;
        corrente = potencia / tensao;
        energia += (potencia / 1000.0) * horas;
    } else {
        tensao = 0.0;
        corrente = 0.0;
        potencia = 0.0;
    }

    ultimaAtualizacao = agora;
}

float obterTensao() { return tensao; }
float obterCorrente() { return corrente; }
float obterPotencia() { return potencia; }
float obterEnergia() { return energia; }
bool cargaLigada() { return cargaLigadaEstado; }
