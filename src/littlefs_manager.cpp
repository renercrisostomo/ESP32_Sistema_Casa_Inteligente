#include "littlefs_manager.h"
#include <LittleFS.h>

const char* DISTANCE_LOG_FILE = "/distance_sensor_log.txt";
const char* ACCESS_LOG_FILE = "/access_log.txt";
const char* USERS_FILE = "/users.txt"; // Novo arquivo para usuários autorizados

void setupLittleFS() {
  if (!LittleFS.begin(true)) { 
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

// Verifica se a tag RFID está na lista de usuários autorizados
bool isUserAuthorized(const String& rfidTag) {
  if (rfidTag.isEmpty()) return false;
  File file = LittleFS.open(USERS_FILE, "r");
  if (!file) {
    // Serial.print("Arquivo de usuários não encontrado: "); // Opcional: log menos verboso
    // Serial.println(USERS_FILE);
    return false; // Se o arquivo não existe, nenhum usuário está autorizado
  }
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim(); 
    if (line == rfidTag) {
      file.close();
      return true;
    }
  }
  file.close();
  return false;
}

// Adiciona uma tag RFID à lista de usuários autorizados
bool addUser(const String& rfidTag) {
  if (rfidTag.isEmpty()) {
    Serial.println("Tentativa de adicionar tag vazia. Operação cancelada.");
    return false;
  }
  if (isUserAuthorized(rfidTag)) {
    Serial.print("Tag [");
    Serial.print(rfidTag);
    Serial.println("] já está cadastrada.");
    return true; // Indica que a tag já existe, não necessariamente um erro
  }
  File file = LittleFS.open(USERS_FILE, "a");
  if (!file) {
    Serial.print("Falha ao abrir ");
    Serial.print(USERS_FILE);
    Serial.println(" para anexar.");
    return false;
  }
  file.println(rfidTag);
  file.close();
  Serial.print("Tag [");
  Serial.print(rfidTag);
  Serial.print("] adicionada a ");
  Serial.println(USERS_FILE);
  return true;
}

// Remove uma tag RFID da lista de usuários autorizados
bool removeUser(const String& rfidTag) {
  if (rfidTag.isEmpty()) {
    Serial.println("Tentativa de remover tag vazia. Operação cancelada.");
    return false;
  }
  // Verifica se o usuário existe antes de tentar remover para evitar trabalho desnecessário
  if (!isUserAuthorized(rfidTag)) {
    Serial.print("Tag [");
    Serial.print(rfidTag);
    Serial.println("] não encontrada para remoção.");
    return false;
  }

  File originalFile = LittleFS.open(USERS_FILE, "r");
  if (!originalFile) {
    Serial.print("Falha ao abrir ");
    Serial.print(USERS_FILE);
    Serial.println(" para leitura durante a remoção.");
    return false; // Não deveria acontecer se isUserAuthorized passou
  }

  const char* TEMP_USERS_FILE = "/users_temp.txt";
  File tempFile = LittleFS.open(TEMP_USERS_FILE, "w");
  if (!tempFile) {
    Serial.print("Falha ao abrir arquivo temporário ");
    Serial.print(TEMP_USERS_FILE);
    Serial.println(" para escrita.");
    originalFile.close();
    return false;
  }

  bool removed = false;
  while (originalFile.available()) {
    String line = originalFile.readStringUntil('\n');
    line.trim();
    if (line == rfidTag) {
      removed = true;
      // Não escreve a linha no arquivo temporário, efetivamente removendo-a
    } else if (!line.isEmpty()) { // Evita escrever linhas vazias se houver alguma no arquivo original
      tempFile.println(line);
    }
  }
  originalFile.close();
  tempFile.close();

  if (removed) {
    if (!LittleFS.remove(USERS_FILE)) {
      Serial.print("Falha ao remover o arquivo original: ");
      Serial.println(USERS_FILE);
      LittleFS.remove(TEMP_USERS_FILE); // Tenta limpar o temporário
      return false;
    }
    if (!LittleFS.rename(TEMP_USERS_FILE, USERS_FILE)) {
      Serial.print("Falha ao renomear arquivo temporário para ");
      Serial.println(USERS_FILE);
      // Se o rename falhar, o arquivo original foi removido. Tentar restaurar é complexo.
      // O mais seguro é deixar o _temp.txt para inspeção manual ou tentar remover.
      return false;
    }
    Serial.print("Tag [");
    Serial.print(rfidTag);
    Serial.print("] removida de ");
    Serial.println(USERS_FILE);
  } else {
    // Este caso não deveria ser alcançado se isUserAuthorized retornou true.
    Serial.print("Tag [");
    Serial.print(rfidTag);
    Serial.println("] não foi encontrada durante o processo de reescrita (inesperado).");
    LittleFS.remove(TEMP_USERS_FILE); // Limpa o arquivo temporário
  }
  return removed;
}

// Lista todas as tags RFID de usuários registrados
String listRegisteredUsers() {
  String userList = "Usuários Cadastrados:\n";
  File file = LittleFS.open(USERS_FILE, "r");
  if (!file) {
    // Serial.print("Arquivo de usuários não encontrado ao listar: "); // Opcional
    // Serial.println(USERS_FILE);
    return "Nenhum usuario cadastrado ou falha ao ler o arquivo.";
  }
  bool usersFound = false;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (!line.isEmpty()) {
        userList += "- " + line + "\n";
        usersFound = true;
    }
  }
  file.close();
  if (!usersFound) {
    return "Nenhum usuario cadastrado.";
  }
  return userList;
}

// Modificada: A verificação de autorização agora é feita ANTES de chamar esta função.
void logAccessAttempt(const String& rfidTag, bool isAuthorized) {
  File file = LittleFS.open(ACCESS_LOG_FILE, "a");
  if (!file) {
    Serial.print("Falha ao abrir ");
    Serial.print(ACCESS_LOG_FILE);
    Serial.println(" para anexar.");
    return;
  }

  // Considere adicionar um timestamp aqui
  // Exemplo: file.print(String(now()) + ","); 
  file.print("Tag: ");
  file.print(rfidTag);
  file.print(", Acesso: ");
  if (isAuthorized) {
    file.println("Autorizado");
  } else {
    file.println("Negado");
  }
  file.close();
}

