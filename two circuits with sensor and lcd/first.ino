#include <Wire.h>

void setup()
{
 	Serial.begin(9600);
 	Wire.begin();
}

void loop()
{
	int reading = analogRead(0);
    float voltage = reading * 5.0;
    voltage /= 1024.0;
    // Температура
    float temperatureC = (voltage - 0.5) * 100;
  	Serial.println(temperatureC);
    delay(1000);
}