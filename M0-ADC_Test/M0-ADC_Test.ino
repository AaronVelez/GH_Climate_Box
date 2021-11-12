/*
 Name:		M0_ADC_Test.ino
 Created:	6/23/2020 12:13:43 AM
 Author:	aivel
*/


int Battery_PIN = A2;


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);

	analogReadResolution(12);
	analogWrite(A0, (1.75 * 1024) / 3.3);

}

// the loop function runs over and over again until power down or reset
void loop() {
	delay(1000);
  

	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);
	Serial.println(analogRead(Battery_PIN) * 3.3 / 4095, 6);


	

}
