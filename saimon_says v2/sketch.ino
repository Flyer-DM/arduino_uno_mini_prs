#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "pitches.h"
// Настройка WI-FI соединения
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* host = "dweet.io";
// Пины лампочек
#define RED_LED 19
#define GREEN_LED 18
#define BLUE_LED 5
#define ORANGE_LED 17
#define WHITE_LED 16
// Пины кнопок
#define RED_BUTTON 12
#define GREEN_BUTTON 14
#define BLUE_BUTTON 27
#define ORANGE_BUTTON 26
#define WHITE_BUTTON 25
// пин звукового выхода
#define SPEAKER_PIN 23
// максимальная последовательность цветов
const int SEQ_LEN = 50;
// скорость переключения лампочек в мс
const int GAME_SPEED = 300;
// один из статусов игры (по умолчанию - ожидание запуска)
int STATUSSS = 0;
// шаг игры (кол-во лампочек)
int GAME_STEP = 0;
// случайная последовательность лампочек
int SEQ[SEQ_LEN];
// текущий шаг игры
int CURR_STEP = 0;
// Подключение дисплея
LiquidCrystal_I2C LCD(0x27,16,2); 

String url;


void setup() {
  Serial.begin(9600);
  // Настройка пинов лампочек на выход
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(ORANGE_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);
  // Настройка пинов кнопок НА считывание входных сигналов
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  pinMode(ORANGE_BUTTON, INPUT_PULLUP);
  pinMode(WHITE_BUTTON, INPUT_PULLUP);
  // Настройка звука
  pinMode(SPEAKER_PIN, OUTPUT);
  // Настройка дисплея
  LCD.init(); 
  LCD.backlight();
  // Подключение к сети
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }  
  Serial.println("Connected!");
}


void loop() {
	switch(STATUSSS){
		case 0:
      		// Ожидания запуска игры
			wait();
			break;
    	case 1:
      		// Генерация и показ последовательности
			show_seq();
			break;
      	case 2:
      		// Ожидание и ввод ответа
			wait_for_answer();
			break;
    } 
}
 

void wait () {
  	// Функция ожидания запуска игры по нажатию белой кнопки
	int start = -1;
	while(start == -1) {
    	LCD.setCursor(0, 0);
  		LCD.print("To start push right button");
        for (int i = 0; i < 16; i++) {
          if(digitalRead(WHITE_BUTTON) == LOW) {
            LCD.clear();
            break;
          }
          LCD.scrollDisplayLeft(); 
          delay(100);
  		} 
    	if(digitalRead(WHITE_BUTTON) == LOW) {
      		start = 0;
          	digitalWrite(WHITE_LED, HIGH);
          	LCD.clear();
          	LCD.setCursor(0, 0);
          	LCD.print("Started");
          	delay(1000);
          	LCD.clear();
          	LCD.setCursor(0, 1);
            LCD.print("Score: ");
          	STATUSSS = 1;
        }
    }
}

void show_seq() {
  	// Функция генерации и показа случайной последовательности
  	delay(GAME_SPEED);
	for (int i = 0; i < GAME_STEP + 1; i++){
    	SEQ[i] = random(4) + 1;
      	delay(GAME_SPEED);
      	showLED(SEQ[i]);
      	delay(GAME_SPEED);
      	clearLED();
    }
  	STATUSSS = 2;
}

void wait_for_answer() {
  // Функция ожидания ввода последовательности пользователем
  int buttonValue = readButton();
  // Если пользователь пока не нажал ни на одну из кнопок
  if (buttonValue != 0) {
    // Если пользователь не нажал на белую кнопку
    if (buttonValue != 5) {
      	showLED(buttonValue);
      	delay(GAME_SPEED);
    	clearLED();
      	// Если пользователь нажал на правильную кнопку
      	if (buttonValue == SEQ[CURR_STEP]){
          	// Если последовательность окончена верно
            if (CURR_STEP == GAME_STEP) {
                tone(SPEAKER_PIN, NOTE_E4);
                delay(150);
                tone(SPEAKER_PIN, NOTE_G4);
                delay(150);
                tone(SPEAKER_PIN, NOTE_E5);
                delay(150);
                tone(SPEAKER_PIN, NOTE_C5);
                delay(150);
                tone(SPEAKER_PIN, NOTE_D5);
                delay(150);
                tone(SPEAKER_PIN, NOTE_G5);
                delay(150);
                noTone(SPEAKER_PIN);
				        GAME_STEP++;
              	CURR_STEP = 0;
              	showAllLED();
              	delay(GAME_SPEED);
              	clearLED();
              	LCD.setCursor(0, 1);
              	LCD.print("Score: ");
              	LCD.print(GAME_STEP);
              	STATUSSS = 1;
            }
          	// Переход к следующему шагу в массиве, если угадал предыдущий
          	else {
        		CURR_STEP++;
          	}
      	}
      	// Если допустил ошибку, то конец игры
      	else {
          tone(SPEAKER_PIN, NOTE_DS5);
          delay(300);
          tone(SPEAKER_PIN, NOTE_D5);
          delay(300);
          tone(SPEAKER_PIN, NOTE_CS5);
          delay(300);
          for (byte i = 0; i < 10; i++) {
            for (int pitch = -10; pitch <= 10; pitch++) {
              tone(SPEAKER_PIN, NOTE_C5 + pitch);
              delay(6);
            }
          }
          noTone(SPEAKER_PIN);
        	STATUSSS = 0;
          restart_game();
      	}
      delay(10);
    }
    // Если пользователь нажал на белую кнопку, перезапуск игры
    else {
      restart_game();
    }
    delay(10);
  }
  delay(10);
}

void showLED(int id) {
  // Функция отображения одной из 4 лампочек
  switch(id) {
    case 1:
      tone(SPEAKER_PIN, NOTE_G3);
      delay(300);
      digitalWrite(RED_LED, HIGH);
      noTone(SPEAKER_PIN);
      break;
    case 2:
      tone(SPEAKER_PIN, NOTE_C4);
      delay(300);
      digitalWrite(GREEN_LED, HIGH);
      noTone(SPEAKER_PIN);
      break;
    case 3:
      tone(SPEAKER_PIN, NOTE_E4);
      delay(300);
      digitalWrite(BLUE_LED, HIGH);
      noTone(SPEAKER_PIN);
      break;
    case 4:
      tone(SPEAKER_PIN, NOTE_G5);
      delay(300);
      digitalWrite(ORANGE_LED, HIGH);
      noTone(SPEAKER_PIN);
      break;
  }
}

void clearLED() {
  // Функция выключения всех лампочек после показа одной
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(ORANGE_LED, LOW);
}

void showAllLED() {
  // Функция включения всех лампочек после правильной последовательности
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(ORANGE_LED, HIGH);
}


int readButton() {
  if (digitalRead(RED_BUTTON) == 0) return 1;
  else if (digitalRead(GREEN_BUTTON) == 0) return 2;
  else if (digitalRead(BLUE_BUTTON) == 0) return 3;
  else if (digitalRead(ORANGE_BUTTON) == 0) return 4;
  else if (digitalRead(WHITE_BUTTON) == 0) return 5;
  return 0;
}

void restart_game() {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Game Over!");
  delay(2000);
  LCD.clear();
  WiFiClient client;
    if(!client.connect(host, 80)){
      return;
    }
  unsigned long timestamp = millis();
  url = "/dweet/for/DmitryKondrashov?Record=" + String(GAME_STEP) + "&timestamp=" + String(timestamp);
  Serial.print("URL: "); 
  Serial.println(url);
  client.print("GET " + url + " HTTP/1.1\r\n" + "Host: " + host +"\r\n" + "Connection: close\r\n\r\n");
  client.stop();

  GAME_STEP = 0;
  CURR_STEP = 0;
  digitalWrite(WHITE_LED, LOW);
  showAllLED();
  delay(GAME_SPEED);
  clearLED();
  delay(GAME_SPEED);
  STATUSSS = 0;
}