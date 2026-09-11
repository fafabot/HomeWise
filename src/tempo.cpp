#include <Arduino.h>

#include "config.h"
#include "tempo.h"

static int diaAtual = 1;
static unsigned long momentoInicioDia = 0;
static bool tempoAtivo = true;

void iniciarTempo() {
    diaAtual = 1;
    momentoInicioDia = millis();
    tempoAtivo = true;
}

bool verificarMudancaDeDia() {
    if (!tempoAtivo) {
        return false;
    }

    unsigned long tempoAtual = millis();
    unsigned long tempoDecorrido = tempoAtual - momentoInicioDia;

    if (tempoDecorrido < SIMULATED_DAY_DURATION_MS) {
        return false;
    }

    momentoInicioDia = tempoAtual;

    if (diaAtual < SIMULATED_DAYS_IN_MONTH) {
        diaAtual++;
    } else {
        tempoAtivo = false;
    }

    return true;
}

int obterDiaAtual() {
    return diaAtual;
}

int obterDiasNoMesAtual() {
    return SIMULATED_DAYS_IN_MONTH;
}

