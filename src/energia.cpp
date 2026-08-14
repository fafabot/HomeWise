#include "energia.h"
#include "config.h"

#include <Arduino.h>

// Nesta primeira etapa, os valores são simulados.
// Depois substituiremos esta parte pela leitura real do PZEM.

float tensao = 0.0;
float corrente = 0.0;
float potencia = 0.0;
float energia = 0.0;

unsigned long ultimaAtualizacao = 0;

void iniciarEnergia() {

    ultimaAtualizacao = millis();

    Serial.println("Sistema de energia iniciado.");
}

void atualizarEnergia() {

    /*
       SIMULAÇÃO

       Representa uma carga semelhante ao liquidificador.

       Futuramente:
       PZEM -> leitura real de tensão,
       corrente, potência e energia.
    */

    tensao = 127.0;
    potencia = 500.0;

    corrente = potencia / tensao;

    unsigned long agora = millis();

    float horas =
        (agora - ultimaAtualizacao) / 3600000.0;

    energia += (potencia / 1000.0) * horas;

    ultimaAtualizacao = agora;
}

float obterTensao() {
    return tensao;
}

float obterCorrente() {
    return corrente;
}

float obterPotencia() {
    return potencia;
}

float obterEnergia() {
    return energia;
}