#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "comunicacao.h"
#include "config.h"
#include "credenciais.h"

static unsigned long ultimaTentativaWiFi = 0;
static unsigned long ultimoEnvioSimulado = 0;
static unsigned long contadorDados = 0;

constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 10000UL;

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

    if (agora - ultimoEnvioSimulado < API_SEND_INTERVAL_MS) {
        return;
    }

    ultimoEnvioSimulado = agora;

    DadosComunicacao dados = obterDadosSimulados();

    Serial.println();
    Serial.println("[COMUNICACAO] Enviando dados para a API...");

    String payload = "{";
    payload += "\"agua\":" + String(dados.agua, 2);
    payload += ",\"vazao\":" + String(dados.vazao, 2);
    payload += ",\"energia\":" + String(dados.energia, 3);
    payload += ",\"potencia\":" + String(dados.potencia, 1);
    payload += "}";

    Serial.print("[COMUNICACAO] JSON: ");
    Serial.println(payload);

    WiFiClient client;
    HTTPClient http;

    if (!http.begin(client, HOMEWISE_API_URL)) {
        Serial.println("[COMUNICACAO] ERRO: nao foi possivel iniciar HTTP.");
        return;
    }

    http.addHeader("Content-Type", "application/json");

    int codigoHTTP = http.POST(payload);

    Serial.print("[COMUNICACAO] Codigo HTTP: ");
    Serial.println(codigoHTTP);

    if (codigoHTTP > 0) {
        String resposta = http.getString();

        Serial.print("[COMUNICACAO] Resposta da API: ");
        Serial.println(resposta);
    } else {
        Serial.print("[COMUNICACAO] ERRO no POST: ");
        Serial.println(http.errorToString(codigoHTTP));
    }

    http.end();
}
