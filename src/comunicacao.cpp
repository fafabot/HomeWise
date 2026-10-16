#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <time.h>

#include "agua.h"
#include "comunicacao.h"
#include "config.h"
#include "credenciais.h"
#include "energia.h"

static unsigned long ultimaTentativaWiFi = 0;
static unsigned long ultimoEnvioAPI = 0;
static bool conexaoRegistrada = false;

constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 10000UL;

static String obterDataHoraISO() {
    time_t agora = time(nullptr);
    // 1735689600 = 01/01/2025 00:00:00 UTC. Se for menor, NTP ainda nao sincronizou
    if (agora < 1735689600) {
        return "2026-10-15T00:00:00Z";
    }
    struct tm* tmInfo = gmtime(&agora);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", tmInfo);
    return String(buffer);
}

void iniciarComunicacao() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Sincroniza o relogio interno com servidores NTP mundiais (UTC)
    configTime(0, 0, "pool.ntp.org", "time.google.com");

    Serial.println();
    Serial.println("[COMUNICACAO] Conectando ao Wi-Fi...");
    ultimaTentativaWiFi = millis();
}

void atualizarComunicacao() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!conexaoRegistrada) {
            conexaoRegistrada = true;
            Serial.print("[COMUNICACAO] Wi-Fi conectado! IP do ESP: ");
            Serial.println(WiFi.localIP());
            Serial.print("[COMUNICACAO] Destino das leituras: Google Firebase Firestore (Nuvem)");
            Serial.println();
        }
        return;
    }

    conexaoRegistrada = false;
    const unsigned long agora = millis();

    if (agora - ultimaTentativaWiFi >= WIFI_RECONNECT_INTERVAL_MS) {
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

    // Aguarda sincronizacao do relogio NTP para garantir data e hora reais na nuvem
    if (time(nullptr) < 1735689600) {
        return;
    }

    ultimoEnvioAPI = agora;

    // Monta o documento no formato nativo da Firestore REST API
    StaticJsonDocument<768> doc;
    JsonObject fields = doc.createNestedObject("fields");

    fields.createNestedObject("dispositivo_id")["stringValue"] = DISPOSITIVO_ID;
    fields.createNestedObject("consumo_agua_litros")["doubleValue"] = obterLitros();
    fields.createNestedObject("vazao_l_min")["doubleValue"] = obterVazao();
    fields.createNestedObject("potencia_w")["doubleValue"] = obterPotencia();
    fields.createNestedObject("energia_kwh")["doubleValue"] = obterEnergia();
    fields.createNestedObject("tensao_v")["doubleValue"] = obterTensao();
    fields.createNestedObject("criado_em")["timestampValue"] = obterDataHoraISO();

    String payload;
    serializeJson(doc, payload);

    Serial.println();
    Serial.println("[COMUNICACAO] Enviando leitura direta para o Firebase Firestore (Nuvem)...");
    Serial.print("[COMUNICACAO] Payload: ");
    Serial.println(payload);

    // Cliente seguro HTTPS (BearSSL) do ESP8266
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure(); // Permite negociar TLS com a Google sem hardcode de certificados CA
    client->setBufferSizes(1024, 512); // Economiza ~20KB de RAM limitando buffers de SSL

    HTTPClient http;
    http.setTimeout(10000); // 10s de timeout para negociacao TLS e envio

    const String firestoreUrl = String("https://firestore.googleapis.com/v1/projects/") +
                               FIREBASE_PROJECT_ID +
                               "/databases/(default)/documents/leituras?key=" +
                               FIREBASE_API_KEY;

    if (!http.begin(*client, firestoreUrl)) {
        Serial.println("[COMUNICACAO] ERRO: Nao foi possivel iniciar conexao HTTPS.");
        return;
    }

    http.addHeader("Content-Type", "application/json");

    const int codigoHTTP = http.POST(payload);
    Serial.print("[COMUNICACAO] Codigo HTTP retornado pelo Firestore: ");
    Serial.println(codigoHTTP);

    if (codigoHTTP >= 200 && codigoHTTP < 300) {
        Serial.println("[COMUNICACAO] SUCESSO: Leitura gravada diretamente no Firebase Firestore!");
    } else if (codigoHTTP > 0) {
        Serial.print("[COMUNICACAO] Resposta da API Firebase: ");
        Serial.println(http.getString());
    } else {
        Serial.print("[COMUNICACAO] Erro no transporte HTTPS: ");
        Serial.println(http.errorToString(codigoHTTP));
    }

    http.end();
}
