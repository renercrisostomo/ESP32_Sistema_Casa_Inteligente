#ifndef LITTLEFS_MANAGER_H
#define LITTLEFS_MANAGER_H

#include <Arduino.h>

// Filenames
extern const char* DISTANCE_LOG_FILE; // Tornar acessível externamente se necessário em outros lugares, ou manter privado
extern const char* ACCESS_LOG_FILE;
extern const char* USERS_FILE;

// Initializes the LittleFS file system
void setupLittleFS();

// Logs distance and water level measurements
void logDistanceMeasurement(float distancia, float nivelAgua);

// Logs access attempts (authorized or unauthorized)
void logAccessAttempt(const String& rfidTag, bool isAuthorized);

// Checks if a user's RFID tag is in the authorized list
bool isUserAuthorized(const String& rfidTag);

// Adds a user's RFID tag to the authorized list
bool addUser(const String& rfidTag);

// Removes a user's RFID tag from the authorized list
bool removeUser(const String& rfidTag);

// Lists all registered user RFID tags
String listRegisteredUsers(); // Retorna uma String para ser enviada pelo Telegram

#endif // LITTLEFS_MANAGER_H
