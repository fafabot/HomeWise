#include "analise.h"
#include "historico.h"

float calcularMediaAgua() {
    int diasFinalizados = obterDiasFinalizados();
    if (diasFinalizados == 0) {
        return 0.0;
    }
    
    float somaAgua = 0.0;
    for (int i = 1; i <= diasFinalizados; i++) {
        somaAgua += obterAguaDoDia(i);
    }
    
    return somaAgua / diasFinalizados;
}

float calcularMediaEnergia() {
    int diasFinalizados = obterDiasFinalizados();
    if (diasFinalizados == 0) {
        return 0.0;
    }
    
    float somaEnergia = 0.0;
    for (int i = 1; i <= diasFinalizados; i++) {
        somaEnergia += obterEnergiaDoDia(i);
    }
    
    return somaEnergia / diasFinalizados;
}

float calcularPrevisaoMensalAgua() {
    return calcularMediaAgua() * obterDiasNoMes();
}

float calcularPrevisaoMensalEnergia() {
    return calcularMediaEnergia() * obterDiasNoMes();
}

float calcularMediaAguaAntesDoDia(int dia) {
    if (dia <= 1 || dia > obterDiasNoMes()) {
        return 0.0;
    }

    int diasFinalizados = obterDiasFinalizados();

    float somaAgua = 0.0;
    int diasUsados = 0;

    for (int i = 1; i < dia && i <= diasFinalizados; i++) {
        somaAgua += obterAguaDoDia(i);
        diasUsados++;
    }

    if (diasUsados == 0) {
        return 0.0;
    }

    return somaAgua / diasUsados;
}


bool consumoAguaAnormal(int dia) {
    if (dia <= 1 || dia > obterDiasNoMes()) {
        return false;
    }

    float mediaAntesDoDia = calcularMediaAguaAntesDoDia(dia);
    float consumoAtual = obterAguaDoDia(dia);

    return consumoAtual > (1.20 * mediaAntesDoDia);
}

bool consumoEnergiaAnormal(int dia) {
    if (dia <= 1 || dia > obterDiasNoMes()) {
        return false;
    }

    float mediaAntesDoDia = calcularMediaEnergia();
    float consumoAtual = obterEnergiaDoDia(dia);

    return consumoAtual > (1.20 * mediaAntesDoDia);
}