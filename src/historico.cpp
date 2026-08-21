#include <Arduino.h>
#include "agua.h"

int diaatual = 1;
unsigned long momentoInicioDia;

float aguapdia[30];

void verificarDia() {
  unsigned long tempoAtual = millis();
  unsigned long tempoDecorrido = tempoAtual - momentoInicioDia;

  if (tempoDecorrido >= 10000) {
    aguapdia[diaatual - 1] = obterLitros();
    Serial.print("Dia ");
    Serial.print(diaatual);
    Serial.println(" finalizado!");
    diaatual++;
    
    momentoInicioDia = tempoAtual;
    resetarConsumoDiarioAgua();
    Serial.print("Novo dia: ");
    Serial.println(diaatual);
  }
}