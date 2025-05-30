#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <Arduino.h>

// Definições para o sensor HC-SR04
#define PINO_TRIG 5
#define PINO_ECHO 27

// Pinos do LED RGB para indicar o nível da água
const int redPin = 19;
const int greenPin = 18;
const int bluePin = 21;

// Constante para o sensor de distância
const float DISTANCIA_MAX_CM = 50.0;  // Altura total do reservatório em cm

// Variáveis globais para leituras do sensor
extern float currentDistancia;
extern float currentNivelAgua;
extern bool  sensorReadSuccessfully;

// Declarações de função
void setupRgbLed();
void readDistanceAndControlRgbLed();
void logCurrentDistance();

#endif // DISTANCE_SENSOR_H
