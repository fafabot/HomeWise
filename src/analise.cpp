#include "analise.h"
#include "config.h"
#include "historico.h"

static float calcularMediaAguaNoIntervalo(int ultimoDia) {
    if (ultimoDia < 1 || ultimoDia > obterDiasFinalizados()) {
        return 0.0f;
    }

    float soma = 0.0f;
    for (int dia = 1; dia <= ultimoDia; dia++) {
        soma += obterAguaDoDia(dia);
    }
    return soma / ultimoDia;
}

static float calcularMediaEnergiaNoIntervalo(int ultimoDia) {
    if (ultimoDia < 1 || ultimoDia > obterDiasFinalizados()) {
        return 0.0f;
    }

    float soma = 0.0f;
    for (int dia = 1; dia <= ultimoDia; dia++) {
        soma += obterEnergiaDoDia(dia);
    }
    return soma / ultimoDia;
}

float calcularMediaAgua() {
    return calcularMediaAguaNoIntervalo(obterDiasFinalizados());
}

float calcularMediaEnergia() {
    return calcularMediaEnergiaNoIntervalo(obterDiasFinalizados());
}

float calcularPrevisaoMensalAgua() {
    return calcularMediaAgua() * obterDiasNoMes();
}

float calcularPrevisaoMensalEnergia() {
    return calcularMediaEnergia() * obterDiasNoMes();
}

float calcularMediaAguaAntesDoDia(int dia) {
    return calcularMediaAguaNoIntervalo(dia - 1);
}

float calcularMediaEnergiaAntesDoDia(int dia) {
    return calcularMediaEnergiaNoIntervalo(dia - 1);
}

bool possuiBaseComparacao(int dia) {
    return dia > 1 && dia <= obterDiasFinalizados();
}

bool consumoAguaAnormal(int dia) {
    if (!possuiBaseComparacao(dia)) {
        return false;
    }

    float mediaAnterior = calcularMediaAguaAntesDoDia(dia);
    float consumoAtual = obterAguaDoDia(dia);
    return consumoAtual > ABNORMAL_CONSUMPTION_FACTOR * mediaAnterior;
}

bool consumoEnergiaAnormal(int dia) {
    if (!possuiBaseComparacao(dia)) {
        return false;
    }

    float mediaAnterior = calcularMediaEnergiaAntesDoDia(dia);
    float consumoAtual = obterEnergiaDoDia(dia);
    return consumoAtual > ABNORMAL_CONSUMPTION_FACTOR * mediaAnterior;
}

