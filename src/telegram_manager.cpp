#include <Arduino.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

#include "littlefs_manager.h" // Para addUser, removeUser, listRegisteredUsers

// Token do bot do Telegram
#define BOT_TOKEN "7713973375:AAEibzzuR0ATjuSjvzAme28onz8V-SqpNzE"
#define CHAT_ID "5768620322"

// Inicialização da conexão segura
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Pino para o LED controlado pelo Telegram
const int telegramLedPin = 4;

void setupTelegramManager() {
    pinMode(telegramLedPin, OUTPUT); // Configura o pino do LED do Telegram como saída
    // Você pode adicionar aqui uma mensagem de inicialização para o bot se desejar
    // bot.sendMessage(CHAT_ID, "Bot de gerenciamento da casa iniciado!", "");
}

// Função para tratar mensagens recebidas
void handleNewMessages() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    Serial.print("Mensagens novas no Telegram: ");
    Serial.println(numNewMessages);

    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;
      String from_name = bot.messages[i].from_name;

      if (from_name == "") from_name = "Desconhecido";

      Serial.print("Recebido de ");
      Serial.print(from_name);
      Serial.print(": ");
      Serial.println(text);

      if (text == "/start") {
        String welcome = "Olá, " + from_name + "!\n";
        welcome += "Use os comandos abaixo para gerenciar usuários RFID:\n";
        welcome += "/cadastro <TAG_RFID> - para adicionar um usuário\n";
        welcome += "/remover <TAG_RFID> - para remover um usuário\n";
        welcome += "/listar - para listar todos os usuários cadastrados\n";
        // Adicionar aqui comandos para o LED se desejar, ex:
        // welcome += "/ledon - para ligar o LED de status\n";
        // welcome += "/ledoff - para desligar o LED de status\n";
        bot.sendMessage(chat_id, welcome, "");
      }
      // Comando para cadastrar usuário
      else if (text.startsWith("/cadastro ")) {
        String rfidTag = text.substring(10); // Pega a string após "/cadastro "
        rfidTag.trim(); // Remove espaços extras
        if (rfidTag.length() > 0) {
          if (addUser(rfidTag)) {
            bot.sendMessage(chat_id, "Tag [" + rfidTag + "] cadastrada com sucesso! ✅", "");
          } else {
            // addUser já imprime no Serial se a tag já existe ou se houve falha ao abrir arquivo
            // Podemos enviar uma mensagem mais genérica ou específica baseada no retorno de addUser se necessário
            bot.sendMessage(chat_id, "Falha ao cadastrar tag [" + rfidTag + "]. Verifique o console ou se já existe.", "");
          }
        } else {
          bot.sendMessage(chat_id, "Por favor, forneça uma tag RFID após o comando /cadastro.", "");
        }
      }
      // Comando para remover usuário
      else if (text.startsWith("/remover ")) {
        String rfidTag = text.substring(9); // Pega a string após "/remover "
        rfidTag.trim();
        if (rfidTag.length() > 0) {
          if (removeUser(rfidTag)) {
            bot.sendMessage(chat_id, "Tag [" + rfidTag + "] removida com sucesso! 🗑️", "");
          } else {
            // removeUser já imprime no Serial se a tag não foi encontrada ou se houve falha
            bot.sendMessage(chat_id, "Falha ao remover tag [" + rfidTag + "]. Verifique se a tag existe.", "");
          }
        } else {
          bot.sendMessage(chat_id, "Por favor, forneça uma tag RFID após o comando /remover.", "");
        }
      }
      // Comando para listar usuários
      else if (text == "/listar") {
        String userList = listRegisteredUsers();
        bot.sendMessage(chat_id, userList, "");
      }
      // Comandos para o LED (exemplo, se quiser reativar)
      /*
      else if (text == "/ledon") {
        digitalWrite(telegramLedPin, HIGH);
        bot.sendMessage(chat_id, "LED de status LIGADO! 💡", "");
      }
      else if (text == "/ledoff") {
        digitalWrite(telegramLedPin, LOW);
        bot.sendMessage(chat_id, "LED de status DESLIGADO! ❌", "");
      }
      */
      else {
        // bot.sendMessage(chat_id, "Comando não reconhecido.", ""); // Opcional
      }
    }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}


