#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "agua.h"
#include "comunicacao.h"
#include "config.h"
#include "credenciais.h"
#include "energia.h"

static unsigned long ultimaTentativaWiFi = 0;
static unsigned long ultimoEnvioAPI = 0;
static bool conexaoRegistrada = false;

constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 10000UL;

void iniciarComunicacao() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println();
    Serial.println("[COMUNICACAO] Conectando ao Wi-Fi...");
    ultimaTentativaWiFi = millis();
}

void atualizarComunicacao() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!conexaoRegistrada) {
            conexaoRegistrada = true;
            Serial.print("[COMUNICACAO] Wi-Fi conectado. IP do ESP: ");
            Serial.println(WiFi.localIP());
        }
        return;
    }

    conexaoRegistrada = false;
    const unsigned long agora = millis();

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

void enviarDadosAPI() {
    if (!wifiConectado()) {
        return;
    }

    const unsigned long agora = millis();
    if (agora - ultimoEnvioAPI < API_SEND_INTERVAL_MS) {
        return;
    }
    ultimoEnvioAPI = agora;

    StaticJsonDocument<256> doc;
    doc["dispositivo_id"] = DISPOSITIVO_ID;
    doc["consumo_agua_litros"] = obterLitros();
    doc["vazao_l_min"] = obterVazao();
    doc["potencia_w"] = obterPotencia();
    doc["energia_kwh"] = obterEnergia();
    doc["tensao_v"] = obterTensao();

    String payload;
    serializeJson(doc, payload);

    Serial.println();
    Serial.println("[COMUNICACAO] Enviando leitura real para a API...");
    Serial.print("[COMUNICACAO] JSON: ");
    Serial.println(payload);

    WiFiClient client;
    HTTPClient http;

    if (!http.begin(client, HOMEWISE_API_URL)) {
        Serial.println("[COMUNICACAO] ERRO: nao foi possivel iniciar HTTP.");
        return;
    }

    http.setTimeout(5000);
    http.addHeader("Content-Type", "application/json");

    const int codigoHTTP = http.POST(payload);
    Serial.print("[COMUNICACAO] Codigo HTTP: ");
    Serial.println(codigoHTTP);

    if (codigoHTTP > 0) {
        Serial.print("[COMUNICACAO] Resposta da API: ");
        Serial.println(http.getString());
    } else {
        Serial.print("[COMUNICACAO] Erro no POST: ");
        Serial.println(http.errorToString(codigoHTTP));
    }

    http.end();
}
