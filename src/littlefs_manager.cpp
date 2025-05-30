#include "littlefs_manager.h"
#include <LittleFS.h>

const char* DISTANCE_LOG_FILE = "/distance_sensor_log.txt";
const char* ACCESS_LOG_FILE = "/access_log.txt";

void setupLittleFS() {
  if (!LittleFS.begin(true)) { // true = format if mount failed
    Serial.println("Falha ao montar LittleFS. O sistema de arquivos pode ser formatado.");
  } else {
    Serial.println("LittleFS montado com sucesso.");
  }
}

void logDistanceMeasurement(float distancia, float nivelAgua) {
  File file = LittleFS.open(DISTANCE_LOG_FILE, "a");
  if (!file) {
    Serial.print("Falha ao abrir ");
    Serial.print(DISTANCE_LOG_FILE);
    Serial.println(" para anexar.");
    return;
  }
  
  // Considere adicionar um timestamp aqui se RTC ou NTP estiverem disponíveis no futuro
  // Exemplo: file.print(String(now()) + ","); 
  file.print("Distancia: ");
  file.print(distancia, 2);
  file.print(" cm, Nivel Agua: ");
  file.print(nivelAgua, 2);
  file.println(" cm");
  file.close();
}

void logAccessAttempt(const String& rfidTag, bool isAutorizado, const String& userName) {
  File file = LittleFS.open(ACCESS_LOG_FILE, "a");
  if (!file) {
    Serial.print("Falha ao abrir ");
    Serial.println(ACCESS_LOG_FILE);
    return;
  }

  // Consider adding a timestamp
  // Example: file.print(String(now()) + ",");
  if (isAutorizado) {
    file.print("Acesso AUTORIZADO: Tag [");
    file.print(rfidTag);
    file.print("], Usuario [");
    file.print(userName.isEmpty() ? "N/A" : userName);
    file.println("]");
  } else {
    file.print("Acesso NEGADO: Tag [");
    file.print(rfidTag);
    file.println("]");
  }
  file.close();
}
