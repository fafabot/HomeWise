#include <Arduino.h>

#include "agua.h"
#include "config.h"
#include "energia.h"
#include "historico.h"
#include "tempo.h"

static int diasFinalizados = 0;

static float aguaPorDia[HISTORY_MAX_DAYS] = {};
static float energiaPorDia[HISTORY_MAX_DAYS] = {};

void iniciarHistorico() {
    diasFinalizados = 0;

    for (int i = 0; i < HISTORY_MAX_DAYS; i++) {
        aguaPorDia[i] = 0.0f;
        energiaPorDia[i] = 0.0f;
    }
}

int verificarDia() {
    int diaFinalizado = obterDiaAtual();

    if (!verificarMudancaDeDia()) {
        return 0;
    }

    if (diaFinalizado < 1 || diaFinalizado > HISTORY_MAX_DAYS) {
        return 0;
    }

    aguaPorDia[diaFinalizado - 1] = obterLitros();
    energiaPorDia[diaFinalizado - 1] = obterEnergia();
    diasFinalizados = diaFinalizado;

    if (diaFinalizado < obterDiasNoMesAtual()) {
        resetarConsumoDiarioAgua();
        resetarConsumoDiarioEnergia();
    }

    return diaFinalizado;
}

float obterAguaDoDia(int dia) {
    if (dia < 1 || dia > diasFinalizados) {
        return -1.0f;
    }

    return aguaPorDia[dia - 1];
}

float obterEnergiaDoDia(int dia) {
    if (dia < 1 || dia > diasFinalizados) {
        return -1.0f;
    }

    return energiaPorDia[dia - 1];
}

int obterDiasFinalizados() {
    return diasFinalizados;
}

int obterDiasNoMes() {
    return obterDiasNoMesAtual();
}

