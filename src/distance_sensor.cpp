#include "distance_sensor.h"
#include <LittleFS.h>

float currentDistancia = 0.0;
float currentNivelAgua = 0.0;
bool  sensorReadSuccessfully = false;

void readDistanceAndControlLed() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  long duracao = pulseIn(PINO_ECHO, HIGH);
  // Verifica se pulseIn retornou 0 (timeout) para evitar cálculos com valores inválidos
  if (duracao > 0) {
    float distancia = (duracao * 0.0343) / 2;
    float nivelAgua = DISTANCIA_MAX_CM - distancia;
    
    // Garante que o nível da água não seja negativo nem exceda o máximo
    if (nivelAgua < 0) nivelAgua = 0;
    if (nivelAgua > DISTANCIA_MAX_CM) nivelAgua = DISTANCIA_MAX_CM;

    currentDistancia = distancia;
    currentNivelAgua = nivelAgua;
    sensorReadSuccessfully = true;

    Serial.print("Distância ao nível da água: ");
    Serial.print(distancia);
    Serial.print(" cm | Nível da água: ");
    Serial.print(nivelAgua);
    Serial.println(" cm");

    // Controlar intensidade do LED com base no nível da água
    // Quanto mais água, mais intenso o LED (até 1023 para resolução de 10 bits)
    int intensidade = map(nivelAgua, 0, DISTANCIA_MAX_CM, 0, 1023);
    ledcWrite(0, intensidade);
  } else {
    Serial.println("Erro ao ler o sensor HC-SR04.");
    sensorReadSuccessfully = false;
    ledcWrite(0, 0); // Estou desligando ele se falhar. O que acha?
  }
}

// Função para salvar o log de distância no LittleFS
void logCurrentDistance() {
  if (!sensorReadSuccessfully) { // Só faz o log se a última leitura foi válida
    Serial.println("Leitura do sensor inválida, log não salvo.");
    return;
  }

  File file = LittleFS.open("/distance_sensor_measurements.txt", "a");
  if (!file) {
    Serial.println("Falha ao abrir /distance_sensor_measurements.txt para log.");
    return;
  }
  // Idealmente, adicionar um timestamp aqui se tiver RTC ou NTP
  file.print("Distancia: ");
  file.print(currentDistancia);
  file.print(" cm, Nivel Agua: ");
  file.print(currentNivelAgua);
  file.println(" cm");
  file.close();
  Serial.println("Log de distância salvo em /distance_sensor_measurements.txt");
}
