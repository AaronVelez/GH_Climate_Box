/*
 Name:		Fan_Speed_Test.ino
 Created:	6/24/2020 5:06:17 PM
 Author:	aivel
*/

int incomingByte = 0; // for incoming serial data
float FanSpeed = 0.0; // DAC voltage MIN = 1.2, MAX = 2.6, Auto-shutdown at ??? (need testing still)

float VRef = 3.307;
int FanI_PIN = A1;
float sum = 0;
int n = 10;
float avg = 0;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(28800);
	analogWrite(A0, 0);

	analogReadResolution(12);

}

// the loop function runs over and over again until power down or reset
void loop() {
	if (Serial.available() > 0) {
		FanSpeed = Serial.parseFloat();
		analogWrite(A0, (FanSpeed * 1023) / VRef);
		Serial.println("\n////////////////////");
		Serial.print("\nNew Fan Speed: ");
		Serial.println(FanSpeed);
		Serial.println("\n////////////////////");
	}


	Serial.print("\n\n\n\n\nFan Speed: ");
	Serial.print(FanSpeed);
	Serial.println(" V");

	/*
	Serial.print("\nSense R voltage drop: ");
	Serial.print(analogRead(FanI_PIN) * VRef / 4095 * 1000, 4);
	Serial.println(" mV");
	*/

	
	sum = 0;
	for (int i = 0; i < n; i++) {
		sum += analogRead(FanI_PIN) * VRef / 4095 * 1000;
	}
	avg = sum / n;
	Serial.print("\nSense R avg voltage drop: ");
	Serial.print(avg, 4);
	Serial.println(" mV");

	delay(1000);
}
