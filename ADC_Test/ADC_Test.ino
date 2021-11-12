/*
 Name:		ADC_Test.ino
 Created:	6/2/2020 10:54:30 AM
 Author:	aivel
*/

#include <SDI12_boards.h>
#include <SDI12.h>
#include <AD7193_CS11.h> // Based on original AD7193.h library by Anne Mahaffey with pin 11 as CS
#include <SPI.h>



AD7193 AD7193;



// ADC data variables
unsigned long ch1Data;
float ch1Voltage;
unsigned long chTempData;
float adcTemp = -1; // ADC chip internal temperature



// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);

	AD7193.begin();
	AD7193.AppendStatusValuetoData();
	AD7193.SetPGAGain(1); // e.g. with gain 8, range is voltage reference / gain: 2.5V / 8 = +/- 312.5 mV
	AD7193.SetAveraging(1023); // Sets filter value in Mode Register (MR9 to MR0). Resulting RMS and peak-to-peak noise in Tables 7, 8 and 9 of AD7193 datasheet
	AD7193.Calibrate(); // Calibrate with given PGA settings - need to recalibrate if PGA setting is changed

	analogWrite(A0, (0.1 * 1024) / 3.3); // Set the DAC to 150 mV
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	delay(2000);
	

	ch1Data = (AD7193.ReadADCChannel(1) >> 8); // Read channel measurement data
	float ch1Voltage = AD7193.DataToVoltage(ch1Data); // Convert to voltage
	chTempData = (AD7193.ReadADCChannel(8) >> 8); // Read temperature data from chip registers
	adcTemp = AD7193.TempSensorDataToDegC(chTempData); // Convert temperature data to degC
	Serial.println((String) "ch1: " + ch1Data);
	Serial.println((String) "Ch1V: " + ch1Voltage);
	Serial.print("Ch1V w 7 digits of precision: ");  Serial.println(ch1Voltage, 7); // Print  with 7 digits
	Serial.println((String) "Tempdata: " + chTempData);
	Serial.println((String) "Temp ADC: " + adcTemp);
	Serial.println(chTempData, HEX);

	
}
