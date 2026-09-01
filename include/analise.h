#ifndef ANALISE_H
#define ANALISE_H

float calcularMediaAgua();
float calcularMediaEnergia();

float calcularPrevisaoMensalAgua();
float calcularPrevisaoMensalEnergia();

float calcularMediaAguaAntesDoDia(int dia);
float calcularMediaEnergiaAntesDoDia(int dia);

bool possuiBaseComparacao(int dia);
bool consumoAguaAnormal(int dia);
bool consumoEnergiaAnormal(int dia);

#endif
