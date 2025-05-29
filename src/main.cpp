#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "distance_sensor.h"

// Pinos do LED RGB
const int redPin = 19;
const int greenPin = 18;
const int bluePin = 21;

// Pino do botão
const int buttonPin = 23;

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

// ******* TELEGRAM *********
// Token do bot do Telegram
#define BOT_TOKEN "7713973375:AAEibzzuR0ATjuSjvzAme28onz8V-SqpNzE"
#define CHAT_ID "5768620322"
// Inicialização da conexão segura
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
// ******* TELEGRAM *********


void saveLedState();
void printLedStateFile();
void loadWiFiCredentialsAndConnect();
void handleNewMessages(int numNewMessages);

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("Inicializando Sistema Casa Inteligente...");

  Serial.println("Montando LittleFS...");
  if (!LittleFS.begin()) {
    Serial.println("Erro ao montar LittleFS");
    return;
  }

  loadWiFiCredentialsAndConnect();
  printLedStateFile();

  Serial.println("Inicialização concluída.");
}

void loop() {
  readDistanceAndControlLed();

  if (intervalHasPassed(lastDistanceLogTime, distanceLogInterval)) {
    lastDistanceLogTime = millis();
    logCurrentDistance();
  }

  if (WiFi.status() == WL_CONNECTED) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  }

  delay(1000);
}

// Conecta ao Wi-Fi com base no arquivo de credenciais
void loadWiFiCredentialsAndConnect() {
  Serial.println("Listando arquivos:");
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

// Registra o usuário no arquivo
void saveLedState() {
  File file = LittleFS.open("/users.txt", "a");  // "a" de append
  if (!file) {
    Serial.println("Erro ao abrir /users.txt para salvar estado");
    return;
  }

  file.printf("Usuário: %d\n", ledState);  // Adiciona nova linha no log
  file.close();
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

// Função para tratar mensagens recebidas
void handleNewMessages(int numNewMessages) {
  Serial.print("Mensagens novas: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;
    String chat_id = bot.messages[i].chat_id;
    String from_name = bot.messages[i].from_name;

    if (from_name == "")
      from_name = "Desconhecido";

    if (text == "/start") {
      String welcome = "Olá, " + from_name + "!\n";
      welcome += "Use os comandos abaixo para controlar o LED:\n";
      welcome += "/ledon para ligar\n";
      welcome += "/ledoff para desligar";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/ledon") {
      digitalWrite(redPin, HIGH);
      bot.sendMessage(chat_id, "LED LIGADO! 💡", "");
    }

    if (text == "/ledoff") {
      digitalWrite(redPin, LOW);
      bot.sendMessage(chat_id, "LED DESLIGADO! ❌", "");
    }
  }
}