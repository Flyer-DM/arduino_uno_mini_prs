#include <Thread.h>

// Пины для светодиодов
const int red1Pin = 2;
const int yellow1Pin = 3;
const int green1Pin = 4;
const int red2Pin = 5;
const int yellow2Pin = 6;
const int green2Pin = 7;

// Время работы каждой фазы в миллисекундах
const unsigned long redDuration = 2500;
const unsigned long greenDuration = 2500;
const unsigned long yellowDuration = 1000;

// Объявление потоков для фаз светофоров
Thread firstThread = Thread();
Thread secondThread = Thread();

// Состояние текущего цвета светофора
enum TrafficLightState { RED, YELLOW, GREEN };
TrafficLightState state1 = RED;
TrafficLightState state2 = GREEN;

// Последнее время смены состояния светофора
unsigned long lastChangeTime1 = 0;
unsigned long lastChangeTime2 = 0;

// Функция для включения светодиода
void turnOn(int pin) {
  digitalWrite(pin, HIGH);
}

// Функция для выключения светодиода
void turnOff(int pin) {
  digitalWrite(pin, LOW);
}

// Функция для установки состояний светофора
void setLights(int redPin, int yellowPin, int greenPin, TrafficLightState state) {
  switch (state) {
    case RED:
      turnOn(redPin);
      turnOff(yellowPin);
      turnOff(greenPin);
      break;
    case YELLOW:
      turnOff(redPin);
      turnOn(yellowPin);
      turnOff(greenPin);
      break;
    case GREEN:
      turnOff(redPin);
      turnOff(yellowPin);
      turnOn(greenPin);
      break;
  }
}

// Функция для установки начальных состояний светофоров
void setupLights() {
  pinMode(red1Pin, OUTPUT);
  pinMode(yellow1Pin, OUTPUT);
  pinMode(green1Pin, OUTPUT);
  pinMode(red2Pin, OUTPUT);
  pinMode(yellow2Pin, OUTPUT);
  pinMode(green2Pin, OUTPUT);

  setLights(red1Pin, yellow1Pin, green1Pin, state1);
  setLights(red2Pin, yellow2Pin, green2Pin, state2);
}

// Функция, запускаемая в потоке для светофора 1
void redGreen() {
  unsigned long currentTime = millis();

  if (state1 == RED && currentTime - lastChangeTime1 >= redDuration) {
    state1 = YELLOW;
    lastChangeTime1 = currentTime;
  } else if (state1 == YELLOW && currentTime - lastChangeTime1 >= yellowDuration) {
    state1 = GREEN;
    lastChangeTime1 = currentTime;
  } else if (state1 == GREEN && currentTime - lastChangeTime1 >= greenDuration) {
    state1 = RED;
    lastChangeTime1 = currentTime;
  }

  setLights(red1Pin, yellow1Pin, green1Pin, state1);
}

// Функция, запускаемая в потоке для светофора 2
void greenRed() {
  unsigned long currentTime = millis();

  if (state2 == RED && currentTime - lastChangeTime2 >= redDuration) {
    state2 = GREEN;
    lastChangeTime2 = currentTime;
  } else if (state2 == YELLOW && currentTime - lastChangeTime2 >= yellowDuration) {
    state2 = RED;
    lastChangeTime2 = currentTime;
  } else if (state2 == GREEN && currentTime - lastChangeTime2 >= greenDuration) {
    state2 = YELLOW;
    lastChangeTime2 = currentTime;
  }

  setLights(red2Pin, yellow2Pin, green2Pin, state2);
}

void setup() {
  setupLights();
  firstThread.onRun(redGreen);
  secondThread.onRun(greenRed);
}

void loop() {
  firstThread.run();
  secondThread.run();
}