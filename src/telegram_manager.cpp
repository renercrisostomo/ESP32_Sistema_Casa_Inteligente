#include <Arduino.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

#include "littlefs_manager.h"

// Token do bot do Telegram
#define BOT_TOKEN "7713973375:AAEibzzuR0ATjuSjvzAme28onz8V-SqpNzE"
#define CHAT_ID "5768620322"

// Inicialização da conexão segura
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Pino para o LED controlado pelo Telegram
const int telegramLedPin = 4;

void setupTelegramManager() {
    
}

// Função para tratar mensagens recebidas
void handleNewMessages() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
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
      welcome += "Use os comandos abaixo para controlar o acesso dos usuários:\n";
      welcome += "/cadastro";
      welcome += "/remover";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/cadastro") {
      
      bot.sendMessage(chat_id, "USUÁRIO CADASTRADO COM SUCESSO! 💡", "");
    }

    if (text == "/remover") {
        bot.sendMessage(chat_id, "USUÁRIO REMOVIDO! ❌", "");
    }

    if (text == "/listar") {
        listarUsuarios();

    }
  }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

}


