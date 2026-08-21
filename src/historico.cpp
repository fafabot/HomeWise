#include <Arduino.h>
#include "agua.h"

int diaatual = 1;
unsigned long momentoInicioDia;

float aguapdia[30];

void iniciarHistorico() {
  momentoInicioDia = millis();
}

bool historicoAtivo = true;

void verificarDia() {
  if (!historicoAtivo) {
    return;
  }

  unsigned long tempoAtual = millis();
  unsigned long tempoDecorrido = tempoAtual - momentoInicioDia;

  if (tempoDecorrido >= 10000) {
    aguapdia[diaatual - 1] = obterLitros();

    Serial.print("Dia ");
    Serial.print(diaatual);
    Serial.println(" finalizado!");

    if (diaatual < 30) {
      diaatual++;
      momentoInicioDia = tempoAtual;

      resetarConsumoDiarioAgua();

      Serial.print("Novo dia: ");
      Serial.println(diaatual);

    } else {
      historicoAtivo = false;

      Serial.println("Limite de dias atingido. Historico completo.");
    }
  }
}

float obterAguaDoDia(int dia) {
  if (dia < 1 || dia > 30) {
    return -1.0;
  }

  return aguapdia[dia - 1];
}