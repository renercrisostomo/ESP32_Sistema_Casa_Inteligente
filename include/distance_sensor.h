#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <Arduino.h>

// Definições para o sensor HC-SR04 e LED de nível
// ! Lembrar de conectar em 5V
#define PINO_TRIG 5     // Pino D4 conectado ao TRIG do HC-SR04
#define PINO_ECHO 27    // Pino D2 conectado ao ECHO do HC-SR04
#define ledPin 26       // LED para indicar nível da água

const float DISTANCIA_MAX_CM = 50.0;  // Altura total do reservatório em cm (nível zero da água)

// Variáveis globais para leituras do sensor
extern float currentDistancia;
extern float currentNivelAgua;
extern bool  sensorReadSuccessfully;

void readDistanceAndControlLed();
void logCurrentDistance();

#endif // DISTANCE_SENSOR_H
