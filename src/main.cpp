#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "distance_sensor.h"
#include "littlefs_manager.h"
#include "rfid_reader.h"
#include "telegram_manager.h"

// Variáveis globais para controle de tempo do log de distância
unsigned long lastDistanceLogTime = 0;
const unsigned long distanceLogInterval = 60000; // 1 minuto em milissegundos

// Função para verificar se um intervalo de tempo passou
bool intervalHasPassed(unsigned long startTime, unsigned long interval) {
  return (millis() - startTime >= interval);
}

// Controle do estado do LED
int ledState = 0;
int lastButtonState = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setupWifi();
void saveLedState();
void printLedStateFile();
void handleNewMessages();

void setup() {
  Serial.begin(115200);
  Serial.println("Inicializando Sistema Casa Inteligente...");
  setupRgbLed();
  setupWifi();
  setupTelegramManager();
  setupRFID();
  setupLittleFS();
  Serial.println("Inicialização  concluída.");
}

void loop() {
  readTagRFID();
  readDistanceAndControlRgbLed();
  handleNewMessages();

  if (intervalHasPassed(lastDistanceLogTime, distanceLogInterval)) {
    lastDistanceLogTime = millis();
    if (sensorReadSuccessfully) {
      logDistanceMeasurement(currentDistancia, currentNivelAgua);
    } else {
      Serial.println("Leitura do sensor invalida, log de distancia nao salvo.");
    }
  }

  delay(1000);
}

// Conecta ao Wi-Fi com base no arquivo de credenciais
void setupWifi() {
  Serial.println("Listando arquivo para acesso ao WIFI:");
  File root = LittleFS.open("/");
  while (File file = root.openNextFile()) {
    Serial.print(" - ");
    Serial.println(file.name());
  }

  File file = LittleFS.open("/wifiCredentials.txt", "r");
  if (!file) {
    Serial.println("Falha ao abrir /wifiCredentials.txt");
    return;
  }

  String ssid = file.readStringUntil('\n');
  String password = file.readStringUntil('\n');
  ssid.trim();
  password.trim();
  file.close();

  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.printf("Conectando-se a %s...\n", ssid.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado ao Wi-Fi!");
}

// Leitura conteúdo dos arquivos
void printLedStateFile() {
  File file = LittleFS.open("/led_state.txt", "r");
  if (!file) {
    Serial.println("Erro ao abrir o arquivo");
    return;
  }

  Serial.println("📄 Conteúdo do led_state.txt:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}
