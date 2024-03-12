#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
}

void loop() {
  if (Serial.available() > 0) {
    float temperatureC = Serial.parseFloat();
    lcd.setCursor(0, 1);
    lcd.print(temperatureC);
    lcd.print(" C     ");
    delay(1000);
  }
}