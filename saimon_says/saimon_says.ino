#include <LiquidCrystal_I2C.h>

// Пины лампочек
#define RED_LED 3
#define GREEN_LED 4
#define BLUE_LED 5
#define ORANGE_LED 6
#define WHITE_LED 2
// Пины кнопок
#define RED_BUTTON 9
#define GREEN_BUTTON 10
#define BLUE_BUTTON 11
#define ORANGE_BUTTON 12
#define WHITE_BUTTON 8
// максимальная последовательность цветов
const int SEQ_LEN = 50;
// скорость переключения лампочек в мс
const int GAME_SPEED = 300;
// один из статусов игры (по умолчанию - ожидание запуска)
int STATUS = 0;
// шаг игры (кол-во лампочек)
int GAME_STEP = 0;
// случайная последовательность лампочек
int SEQ[SEQ_LEN];
// текущий шаг игры
int CURR_STEP = 0;
// Подключение дисплея
LiquidCrystal_I2C LCD(0x27,16,2); 



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
  // Настройка дисплея
  LCD.init(); 
  LCD.backlight();  
}


void loop() {
	switch(STATUS){
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
          delay(50);
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
          	STATUS = 1;
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
  	STATUS = 2;
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
          	// Если последовательность окончена
            if (CURR_STEP == GAME_STEP) {
				GAME_STEP++;
              	CURR_STEP = 0;
              	showAllLED();
              	delay(GAME_SPEED);
              	clearLED();
              	LCD.setCursor(0, 1);
              	LCD.print("Score: ");
              	LCD.print(GAME_STEP);
              	STATUS = 1;
            }
          	// Переход к следующему шагу в массиве, если угадал предыдущий
          	else {
        		CURR_STEP++;
          	}
      	}
      	// Если допустил ошибку, то конец игры
      	else {
        	STATUS = 0;
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
      digitalWrite(RED_LED, HIGH);
      break;
    case 2:
      digitalWrite(GREEN_LED, HIGH);
      break;
    case 3:
      digitalWrite(BLUE_LED, HIGH);
      break;
    case 4:
      digitalWrite(ORANGE_LED, HIGH);
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
  LCD.setCursor(0, 1);
  LCD.print("Restart?");
  delay(2000);
  LCD.clear();
  GAME_STEP = 0;
  CURR_STEP = 0;
  digitalWrite(WHITE_LED, LOW);
  showAllLED();
  delay(GAME_SPEED);
  clearLED();
  delay(GAME_SPEED);
  STATUS = 0;
}