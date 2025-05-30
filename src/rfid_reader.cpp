#include <Arduino.h>
#include <LittleFS.h>

#include "littlefs_manager.h"

HardwareSerial RFIDSerial(1); // Usamos a UART1

void setupRFID() {
  Serial.println("Sistema de leitura RFID iniciado...");
  RFIDSerial.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17 (inverter se necessário)
}

void readTagRFID() {
  if (RFIDSerial.available()) {
    String tag = "";
    // Aguarda todos os bytes da tag
    while (RFIDSerial.available()) {
      char c = RFIDSerial.read();
      if (isPrintable(c)) {
        tag += c;
      }
    }
    logAccessAttempt(tag, true);
    Serial.print("Tag detectada: ");
    Serial.println(tag);
    saveUser(tag);
  } else {
    Serial.println("Erro desconhecido na leitura da Tag!");
  }
}


// Registra o usuário no arquivo
void saveUser(String usertag) {
  File file = LittleFS.open("/access_log.txt", "a");  // "a" de append
  if (!file) {
    Serial.println("Erro ao abrir /access_log.txt para salvar estado");
    return;
  }

  file.printf("Tag Usuario: %d\n", usertag);  // Adiciona nova linha no log
  file.close(); 
}