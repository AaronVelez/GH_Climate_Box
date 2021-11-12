/*
 Name:		PID_Test.ino
 Created:	6/29/2020 3:46:02 PM
 Author:	aivel
*/

#include <PID_v1.h>

unsigned long time;
int lastSerial;

int incomingByte = 0; // for incoming serial data
float VRef = 3.307;
int FanI_PIN = A1;
float sum = 0;
int n = 100;

float Kp = 0.00001; // 
float Ki = 0.002; //
float Kd = 0.000001; //

double Setpoint; // in mV of voltage drop across sense resistor
double Input;
double Output;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(28800);
	analogWrite(A0, 0);
	analogReadResolution(12);

	Setpoint = 250;


	myPID.SetOutputLimits(0, VRef);
	myPID.SetMode(AUTOMATIC); //turn the PID on

}

// the loop function runs over and over again until power down or reset
void loop() {
	if (Serial.available() > 0) {
		Setpoint = Serial.parseFloat();
		Serial.println("\n////////////////////");
		Serial.print("\nNew Fan Speed Setpoint: ");
		Serial.println(Setpoint);
		Serial.println("\n////////////////////");
	}
	
	sum = 0;
	for (int i = 0; i < n; i++) {
		sum += analogRead(FanI_PIN) * VRef / 4095 * 1000;
	}
	Input = sum / n;
	myPID.Compute();
	analogWrite(A0, (Output * 1023) / VRef);
	


	time = millis();
	if (time - lastSerial > 1000) {
		Serial.print("\n\n\n\n\nFan Setpoint: ");
		Serial.print(Setpoint);
		Serial.println(" mV");

		Serial.print("\nSense R avg voltage drop: ");
		Serial.print(Input, 4);
		Serial.println(" mV");

		Serial.print("\nPID output: ");
		Serial.print(Output, 4);
		Serial.println(" V");

		lastSerial = time;
	}
	

}
