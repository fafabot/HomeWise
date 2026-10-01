#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "comunicacao.h"
#include "config.h"
#include "credenciais.h"

static unsigned long ultimaTentativaWiFi = 0;
static unsigned long ultimoEnvioSimulado = 0;
static unsigned long contadorDados = 0;

constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 10000UL;
constexpr unsigned long SIMULATED_DATA_INTERVAL_MS = 5000UL;

void iniciarComunicacao() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println();
    Serial.println("[COMUNICACAO] Conectando ao Wi-Fi...");

    ultimaTentativaWiFi = millis();
}

void atualizarComunicacao() {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    unsigned long agora = millis();

    if (agora - ultimaTentativaWiFi >= WIFI_RECONNECT_INTERVAL_MS) {
        Serial.println("[COMUNICACAO] Tentando reconectar ao Wi-Fi...");

        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        ultimaTentativaWiFi = agora;
    }
}

bool wifiConectado() {
    return WiFi.status() == WL_CONNECTED;
}

DadosComunicacao obterDadosSimulados() {
    DadosComunicacao dados;

    contadorDados++;

    dados.agua = 10.0f + (contadorDados % 10) * 0.25f;
    dados.vazao = 2.5f + (contadorDados % 5) * 0.50f;
    dados.energia = 0.80f + (contadorDados % 8) * 0.05f;
    dados.potencia = 350.0f + (contadorDados % 6) * 25.0f;

    return dados;
}

void enviarDadosSimulados() {
    if (!wifiConectado()) {
        return;
    }

    unsigned long agora = millis();

    if (agora - ultimoEnvioSimulado < SIMULATED_DATA_INTERVAL_MS) {
        return;
    }

    ultimoEnvioSimulado = agora;

    DadosComunicacao dados = obterDadosSimulados();

    Serial.println();
    Serial.println("[COMUNICACAO] Dados simulados:");

    Serial.print("Wi-Fi: ");
    Serial.println(WiFi.SSID());

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    Serial.println("JSON simulado:");
    Serial.print("{\"agua\":");
    Serial.print(dados.agua, 2);
    Serial.print(",\"vazao\":");
    Serial.print(dados.vazao, 2);
    Serial.print(",\"energia\":");
    Serial.print(dados.energia, 3);
    Serial.print(",\"potencia\":");
    Serial.print(dados.potencia, 1);
    Serial.println("}");
}
