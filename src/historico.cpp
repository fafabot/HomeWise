#include <Arduino.h>

#include "agua.h"
#include "config.h"
#include "energia.h"
#include "historico.h"

static int diaAtual = 1;
static unsigned long momentoInicioDia = 0;
static bool historicoAtivo = true;

static float aguaPorDia[HISTORY_MAX_DAYS] = {};
static float energiaPorDia[HISTORY_MAX_DAYS] = {};

void iniciarHistorico() {
    diaAtual = 1;
    momentoInicioDia = millis();
    historicoAtivo = true;

    for (int i = 0; i < HISTORY_MAX_DAYS; i++) {
        aguaPorDia[i] = 0.0f;
        energiaPorDia[i] = 0.0f;
    }
}

int verificarDia() {
    if (!historicoAtivo) {
        return 0;
    }

    unsigned long tempoAtual = millis();
    unsigned long tempoDecorrido = tempoAtual - momentoInicioDia;

    if (tempoDecorrido < SIMULATED_DAY_DURATION_MS) {
        return 0;
    }

    int diaFinalizado = diaAtual;
    aguaPorDia[diaFinalizado - 1] = obterLitros();
    energiaPorDia[diaFinalizado - 1] = obterEnergia();

    if (diaAtual < SIMULATED_DAYS_IN_MONTH) {
        diaAtual++;
        momentoInicioDia = tempoAtual;
        resetarConsumoDiarioAgua();
        resetarConsumoDiarioEnergia();
    } else {
        historicoAtivo = false;
    }

    return diaFinalizado;
}

float obterAguaDoDia(int dia) {
    if (dia < 1 || dia > obterDiasFinalizados()) {
        return -1.0f;
    }
    return aguaPorDia[dia - 1];
}

float obterEnergiaDoDia(int dia) {
    if (dia < 1 || dia > obterDiasFinalizados()) {
        return -1.0f;
    }
    return energiaPorDia[dia - 1];
}

int obterDiasFinalizados() {
    return historicoAtivo ? diaAtual - 1 : SIMULATED_DAYS_IN_MONTH;
}

int obterDiasNoMes() {
    return SIMULATED_DAYS_IN_MONTH;
}
