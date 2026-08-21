#ifndef ENERGIA_H
#define ENERGIA_H

void iniciarEnergia();
void atualizarEnergia();
void definirCarga(bool ligada);

float obterTensao();
float obterCorrente();
float obterPotencia();
float obterEnergia();
bool cargaLigada();

void resetarConsumoDiarioEnergia();
#endif
