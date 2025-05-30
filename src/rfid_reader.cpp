#include <Arduino.h>
// #include <LittleFS.h> // Não é mais necessário diretamente aqui se todas as ops de FS estão no manager

#include "littlefs_manager.h" // Para isUserAuthorized e logAccessAttempt

HardwareSerial RFIDSerial(1); // Usamos a UART1

void setupRFID() {
  Serial.println("Sistema de leitura RFID iniciado...");
  RFIDSerial.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17 (Pinos para ESP32 UART1)
}

void readTagRFID() {
  if (RFIDSerial.available()) {
    String tag = "";
    // Aguarda um pouco para garantir que todos os bytes da tag cheguem
    delay(50); // Pequeno delay para estabilizar a leitura da serial
    while (RFIDSerial.available()) {
      char c = RFIDSerial.read();
      // O RDM6300 envia caracteres de controle no início e fim (STX, ETX, CR, LF)
      // Vamos pegar apenas os caracteres imprimíveis que formam a tag (geralmente hexadecimais)
      if (isGraph(c)) { // isGraph() verifica se é um caractere gráfico (imprimível, exceto espaço)
        tag += c;
      }
    }

    tag.trim(); // Remove espaços em branco extras, se houver

    if (tag.length() > 0) { // Verifica se alguma tag foi realmente lida
      Serial.print("Tag Detectada: [");
      Serial.print(tag);
      Serial.println("]");

      bool autorizado = isUserAuthorized(tag);
      logAccessAttempt(tag, autorizado);

      if (autorizado) {
        Serial.println("ACESSO AUTORIZADO!");
        // TODO: Adicionar lógica para acionar o relé aqui
        // Ex: digitalWrite(PINO_RELE, HIGH);
        //     delay(1000); // Mantém o relé acionado por 1 segundo
        //     digitalWrite(PINO_RELE, LOW);
      } else {
        Serial.println("ACESSO NEGADO.");
      }
    } 
    // Removido o else que imprimia "Erro desconhecido na leitura da Tag!"
    // pois RFIDSerial.available() ser falso é o estado normal quando não há tag.
  } 
  // Removida a função saveUser(String usertag)
  // O cadastro de usuários será feito via Telegram usando addUser() de littlefs_manager.cpp
}