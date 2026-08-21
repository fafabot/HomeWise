#include <Arduino.h>

int diaatual = 1;
unsigned long momentoInicioDia;

void verificarDia() {
  unsigned long tempoAtual = millis();
  unsigned long tempoDecorrido = tempoAtual - momentoInicioDia;

  if (tempoDecorrido >= 10000) {
    diaatual++;

    momentoInicioDia = tempoAtual;

    Serial.print("Novo dia: ");
    Serial.println(diaatual);
  }
}