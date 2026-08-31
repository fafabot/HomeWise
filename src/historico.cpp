#include <Arduino.h>
#include "agua.h"
#include "energia.h"

const int MAX_DIAS_MES = 31;
int diasNoMes = 30;

int diaatual = 1;
unsigned long momentoInicioDia;

float aguapdia[MAX_DIAS_MES];
float energiapdia[MAX_DIAS_MES];

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
    energiapdia[diaatual - 1] = obterEnergia();

    Serial.print("Dia ");
    Serial.print(diaatual);
    Serial.println(" finalizado!");

    if (diaatual < diasNoMes) {
      diaatual++;
      momentoInicioDia = tempoAtual;

      resetarConsumoDiarioEnergia();
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
  if (dia < 1 || dia > diasNoMes) {
    return -1.0;
  }

  return aguapdia[dia - 1];
}

float obterEnergiaDoDia(int dia) {
  if (dia < 1 || dia > diasNoMes) {
    return -1.0;
  }

  return energiapdia[dia - 1];
}

int obterDiasFinalizados() {
  if (historicoAtivo) {
    return diaatual - 1;
  }

  return diasNoMes;
}
