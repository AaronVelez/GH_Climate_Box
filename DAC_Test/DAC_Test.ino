/*
 Name:		DAC_Test.ino
 Created:	6/17/2020 4:40:34 PM
 Author:	aivel
*/

// the setup function runs once when you press reset or power the board
void setup() {
	analogWrite(A0, (0.15*1024)/3.3);
	
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
