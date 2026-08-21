#include <Arduino.h>
#include "agua.h"
#include "energia.h"

int diaatual = 1;
unsigned long momentoInicioDia;

float aguapdia[30];
float energopdia[30];

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
    energopdia[diaatual - 1] = obterEnergia();

    Serial.print("Dia ");
    Serial.print(diaatual);
    Serial.println(" finalizado!");

    if (diaatual < 30) {
      diaatual++;
      momentoInicioDia = tempoAtual;

      resetarConsumoDiarioAgua();
      resetarConsumoDiarioEnergia();

      Serial.print("Novo dia: ");
      Serial.println(diaatual);

    } else {
      historicoAtivo = false;

      Serial.println("Limite de dias atingido. Historico completo.");
    }
  }
}