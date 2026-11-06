#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <time.h>

#include "agua.h"
#include "comunicacao.h"
#include "config.h"

#if __has_include("credenciais.h")
#include "credenciais.h"
#elif __has_include("credenciais.example.h")
#include "credenciais.example.h"
#endif

#include "energia.h"

static unsigned long ultimaTentativaWiFi = 0;
static unsigned long ultimoEnvioAPI = 0;
static bool conexaoRegistrada = false;

constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 10000UL;
constexpr int MAX_FILA_OFFLINE = 10;

struct LeituraBufferOffline {
    float consumoAguaLitros;
    float vazaoLMin;
    float potenciaW;
    float energiaKwh;
    float tensaoV;
    char criadoEm[25];
};

static LeituraBufferOffline filaOffline[MAX_FILA_OFFLINE];
static int inicioFila = 0;
static int quantidadeFila = 0;

static void enfileirarLeituraOffline(const LeituraBufferOffline& item) {
    if (quantidadeFila < MAX_FILA_OFFLINE) {
        const int indice = (inicioFila + quantidadeFila) % MAX_FILA_OFFLINE;
        filaOffline[indice] = item;
        quantidadeFila++;
    } else {
        // Fila cheia: substitui a leitura mais antiga mantendo as mais recentes
        filaOffline[inicioFila] = item;
        inicioFila = (inicioFila + 1) % MAX_FILA_OFFLINE;
    }
    Serial.print("[COMUNICACAO] Leitura armazenada no buffer de recuperacao offline (pendentes: ");
    Serial.print(quantidadeFila);
    Serial.println(")");
}

int obterLeiturasPendentesOffline() {
    return quantidadeFila;
}

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

static bool transmitirDocumentoFirestore(const LeituraBufferOffline& leitura) {
    // Monta o documento no formato nativo da Firestore REST API
    StaticJsonDocument<768> doc;
    JsonObject fields = doc.createNestedObject("fields");

    fields.createNestedObject("dispositivo_id")["stringValue"] = DISPOSITIVO_ID;
    fields.createNestedObject("consumo_agua_litros")["doubleValue"] = leitura.consumoAguaLitros;
    fields.createNestedObject("vazao_l_min")["doubleValue"] = leitura.vazaoLMin;
    fields.createNestedObject("potencia_w")["doubleValue"] = leitura.potenciaW;
    fields.createNestedObject("energia_kwh")["doubleValue"] = leitura.energiaKwh;
    fields.createNestedObject("tensao_v")["doubleValue"] = leitura.tensaoV;
    fields.createNestedObject("criado_em")["timestampValue"] = leitura.criadoEm;

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
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    const int codigoHTTP = http.POST(payload);
    Serial.print("[COMUNICACAO] Codigo HTTP retornado pelo Firestore: ");
    Serial.println(codigoHTTP);

    bool sucesso = false;
    if (codigoHTTP >= 200 && codigoHTTP < 300) {
        Serial.println("[COMUNICACAO] SUCESSO: Leitura gravada diretamente no Firebase Firestore!");
        sucesso = true;
    } else if (codigoHTTP > 0) {
        Serial.print("[COMUNICACAO] Resposta da API Firebase: ");
        Serial.println(http.getString());
    } else {
        Serial.print("[COMUNICACAO] Erro no transporte HTTPS: ");
        Serial.println(http.errorToString(codigoHTTP));
    }

    http.end();
    return sucesso;
}

void enviarDadosAPI() {
    const unsigned long agora = millis();
    if (agora - ultimoEnvioAPI < API_SEND_INTERVAL_MS) {
        return;
    }

    ultimoEnvioAPI = agora;

    LeituraBufferOffline leituraAtual;
    leituraAtual.consumoAguaLitros = obterLitros();
    leituraAtual.vazaoLMin = obterVazao();
    leituraAtual.potenciaW = obterPotencia();
    leituraAtual.energiaKwh = obterEnergia();
    leituraAtual.tensaoV = obterTensao();
    String iso = obterDataHoraISO();
    strncpy(leituraAtual.criadoEm, iso.c_str(), sizeof(leituraAtual.criadoEm) - 1);
    leituraAtual.criadoEm[sizeof(leituraAtual.criadoEm) - 1] = '\0';

    if (!wifiConectado()) {
        Serial.println("[COMUNICACAO] Sem Wi-Fi no momento. Executando procedimento de recuperacao (RF10)...");
        enfileirarLeituraOffline(leituraAtual);
        return;
    }

    // Aguarda sincronizacao do relogio NTP para garantir data e hora reais na nuvem
    if (time(nullptr) < 1735689600) {
        return;
    }

    // Descarrega eventual leitura pendente na fila antes da leitura atual
    if (quantidadeFila > 0) {
        Serial.print("[COMUNICACAO] Conexao restabelecida! Reenviando leitura pendente (restam ");
        Serial.print(quantidadeFila);
        Serial.println(")...");
        if (transmitirDocumentoFirestore(filaOffline[inicioFila])) {
            inicioFila = (inicioFila + 1) % MAX_FILA_OFFLINE;
            quantidadeFila--;
        } else {
            enfileirarLeituraOffline(leituraAtual);
            return;
        }
    }

    if (!transmitirDocumentoFirestore(leituraAtual)) {
        enfileirarLeituraOffline(leituraAtual);
    }
}
