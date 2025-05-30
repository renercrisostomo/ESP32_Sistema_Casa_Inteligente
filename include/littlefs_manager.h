#ifndef LITTLEFS_MANAGER_H
#define LITTLEFS_MANAGER_H

#include <Arduino.h>

void setupLittleFS();

void logDistanceMeasurement(float distancia, float nivelAgua);

// userName é opcional pq pode ser vazio se não houver usuário associado
void logAccessAttempt(const String& rfidTag, bool isAutorizado);

#endif // LITTLEFS_MANAGER_H
