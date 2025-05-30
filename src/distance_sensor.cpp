#include "distance_sensor.h"

// Definições dos canais PWM para o LED RGB
const int redChannel = 0;     // Canal PWM para o LED vermelho
const int greenChannel = 1;   // Canal PWM para o LED verde
const int blueChannel = 2;    // Canal PWM para o LED azul
const int pwmFreq = 5000;     // Frequência do PWM em Hz
const int pwmResolution = 8;  // Resolução em bits (8 bits = 0-255)

// Definição das variáveis globais (elas já são declaradas como extern no .h)
float currentDistancia = 0.0;
float currentNivelAgua = 0.0;
bool  sensorReadSuccessfully = false;

// Função para configurar os pinos e o PWM para o LED RGB
void setupRgbLed() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  ledcSetup(redChannel, pwmFreq, pwmResolution);
  ledcSetup(greenChannel, pwmFreq, pwmResolution);
  ledcSetup(blueChannel, pwmFreq, pwmResolution);

  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  Serial.println("Setup LED RGB.");
}

// Função para ler a distância e controlar o LED RGB
void readDistanceAndControlRgbLed() {
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);

  long duracao = pulseIn(PINO_ECHO, HIGH);
  if (duracao > 0) {
    float distancia = (duracao * 0.0343) / 2;
    float nivelAgua = DISTANCIA_MAX_CM - distancia;
    
    if (nivelAgua < 0) nivelAgua = 0;
    if (nivelAgua > DISTANCIA_MAX_CM) nivelAgua = DISTANCIA_MAX_CM;

    currentDistancia = distancia;
    currentNivelAgua = nivelAgua;
    sensorReadSuccessfully = true;

    Serial.print("Distância: ");
    Serial.print(distancia);
    Serial.print(" cm | Nível: ");
    Serial.print(nivelAgua);
    Serial.println(" cm");

    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;

    // Lógica para mudar a COR do LED RGB com base no nível da água
    if (nivelAgua <= DISTANCIA_MAX_CM * 0.1) { // Nível muito baixo (até 10%)
      redValue = 255; greenValue = 0; blueValue = 0; // Vermelho
    } else if (nivelAgua <= DISTANCIA_MAX_CM * 0.3) { // Nível baixo (10-30%)
      redValue = 255; greenValue = 120; blueValue = 0; // Laranja
    } else if (nivelAgua <= DISTANCIA_MAX_CM * 0.7) { // Nível médio (30-70%)
      redValue = 255; greenValue = 255; blueValue = 0; // Amarelo
    } else if (nivelAgua < DISTANCIA_MAX_CM * 0.9) { // Nível bom (70-90%)
      redValue = 0; greenValue = 255; blueValue = 0;   // Verde
    } else { // Nível cheio (90-100%)
      redValue = 0; greenValue = 150; blueValue = 255; // Azul/Ciano para cheio
    }

    ledcWrite(redChannel, redValue);
    ledcWrite(greenChannel, greenValue);
    ledcWrite(blueChannel, blueValue);

  } else {
    Serial.println("Erro ao ler o sensor HC-SR04.");
    sensorReadSuccessfully = false;
    // Em caso de erro, apaga o LED
    ledcWrite(redChannel, 0);
    ledcWrite(greenChannel, 0);
    ledcWrite(blueChannel, 0);
  }
}
