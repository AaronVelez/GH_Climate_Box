/*
 Name:		RH_Test.ino
 Created:	6/1/2020 10:40:48 AM
 Author:	aivel
*/


#include "Wire.h"




float Temp = 0.0;
float RH = 0.0;


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);  // start serial for output
	Wire.begin();
	Serial.println("Amphenol Advanced Sensors");
	Serial.println("Arduino T9602 Monitor");

}

// the loop function runs over and over again until power down or reset
void loop() {
	getTempRH();
	delay(3000);
}


