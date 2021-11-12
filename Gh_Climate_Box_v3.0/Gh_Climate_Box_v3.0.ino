/*
 Name:		Gh_Climate_Box_v3.0.ino
 Created:	5/29/2020 12:14:01 PM
 Author:	Daniela Cornejo Corona and Aarón I. Vélez Ramírez
 Universidad Nacional Autónoma de México

 Code for ENES León Greenhouse Climate Boxes

 Details:
 
 RTC adjust via Serial
 ADC AD7193 used only for internal temperature data, it uses a modified library to use pin 11 instead of 10
 Fan Speed control via DAC with N-MOSFET and feedback with sense resistor
 Battery voltage monitor with resistor divider

 No xbee comunication yet
 No SDI-12 for soil water content meter yet
 No missing SD card detect yet
*/






////////////////////////////////////////
// Libraries and associated constants //
////////////////////////////////////////

////// Comunication protocols
#include "Wire.h" // I2C
#include <SPI.h> // SPI


////// RTC DS3231
#include <RTClib.h>
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };


////// RTC sync over serial
#include <TimeLib.h>
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message


////// ADC AD7193
#include <AD7193_CS11.h> // Based on original AD7193.h library by Anne Mahaffey with pin 11 as CS
AD7193 AD7193;


////// SD
#include <SD.h>


////// e-ink Display (EPD)
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_MonoOLED.h>
#include <Adafruit_GFX.h>
#include <Adafruit_EPD.h>
// See display selecto¿ion code in after display pins definitions in pin secction


////// Fan speed PID control
#include <PID_v1.h>



//////////
// Pins //
//////////
#define SD_CS_PIN 4 // SD on Addlogger M0 Feather 
#define Debug_LED 13 // Built in LED


#define EPD_CS     9
#define EPD_DC      10
#define SRAM_CS     6
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)
Adafruit_SSD1675 display(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);


int AC_PIN = A1;
int Battery_PIN = A2;
int FanI_PIN = A3;




////////////////////
// User constants //
////////////////////

////// Station IDs & Constants
int StaNum = 1;
String StaType = String("Gh Climate Box");
String StaName = String("Gh Climate Box Compartment 2");
String Firmware = String("Gh_Climate_Box_v3.0.ino");
float VRef = 3.276;
// Xbee ID, etc 


////// Log File & Headers
File LogFile;
String FileName = "";
String Headers = (String)"year\t" + "month\t" + "day\t" + "hour\t" + "minute\t" + "second\t" + 
						"AirTemp\t" + "AirRH\t" + "ADCTemp\t" + 
						"Fan_I\t" + "AC_OK\t" + "BatteryV\t" + "SentXBEE";
String LogString = "";


////// M0 ADC constants
int n = 100; // measure n times the ADC input for averaging
float sum = 0; // shift register to hold ADC data


////// Debug/Status LED blink
unsigned long LEDtime;
unsigned long lastBlink;
int BlinkTime = 1000;
int LEDstate = HIGH;


////// Fan Speed PID control
double PID_Setpoint = 300; // in mV of voltage drop across 1 ohm sense resistor = mA 
double PID_Input; // in mV of voltage drop across 1 ohm sense resistor = mA
double PID_Output; // in MOSFET Gate voltage (V)
float Kp = 0.00001; 
float Ki = 0.002;
float Kd = 0.000001;
PID fanPID(&PID_Input, &PID_Output, &PID_Setpoint, Kp, Ki, Kd, DIRECT);


////// Time variables
int s = -1; //Seconds
int m = -1; //Minutes
int h = -1; //Hours
int dy = -1; //Day
int mo = -1; //Month
int yr = -1; //Year


////// State machine Shift Registers
int LastSum = -1; // Last minute that variables were added to the sum for later averaging
int SumNum = 0; // Number of times a variable value has beed added to the sum for later averaging
int LastLog = -1; // Last minute that variables were loged to the SD card
boolean LogRdy = 0; // Variable to indicate if there is log data ready to be sent to server
int LastSec = -1; // Last second that Arduino waited for server "go ahead to erase log file"
int ServerWait = 0; // Counter to stop wating server for a "go ahead to erase log file"


////// Measured instantaneous variables
float Temp = -1; // Temp value read each minute
float RH = -1; // RH value read each minute
//float GlobalR = -1; // Global Radiation value read each minute
float adcTemp = -1; // ADC chip internal temperature


////// Variables to store sum for eventual averaging
float TempSum = 0;
float RHSum = 0;
//float GlobalRSum = 0;
float adcTempSum = 0;


////// Values to be logged. They will be the average over the last 5 minutes
float TempAvg = -1;
float RHAvg = -1;
//float GlobalRAvg = -1;
boolean ACok = 1; // State of AC power to main power supply
float BatteryV = -1; // Battery voltage value is NOT averaged in order to log the latest value
float adcTempAvg = -1;


////// ADC data variables
// unsigned long ch1Data; // Not in use now
//float ch1Voltage; // Not in use now
unsigned long chTempData;
bool ADC_Status_Ch8;






//////////////////////////////////////////////////////////////////////////
// the setup function runs once when you press reset or power the board //
//////////////////////////////////////////////////////////////////////////
void setup() {

	////// Start General Libraries
	Serial.begin(115200);
	Wire.begin();
	SPI.begin();
	SD.begin(SD_CS_PIN);
	

	////// Set pin modes of pins not associated with libraries
	analogReadResolution(12);


	////// Setup status/debug LED
	pinMode(Debug_LED, OUTPUT);
	digitalWrite(Debug_LED, LEDstate);


	////// Set Fan Speed
	fanPID.SetOutputLimits(0, VRef);
	fanPID.SetMode(AUTOMATIC); //turn the PID Controller ON
	

	////// RTC setup
	rtc.begin(); // I2C
	setSyncProvider(requestSync); // Set function to call when date and time sync required


	////// ADC Setup
	AD7193.begin();
	AD7193.Reset();
	AD7193.AppendStatusValuetoData();
	AD7193.Calibrate();


	////// e-ink display setup
	display.begin();
#if defined(FLEXIBLE_213) || defined(FLEXIBLE_290)
	// The flexible displays have different buffers and invert settings!
	display.setBlackBuffer(1, false);
	display.setColorBuffer(1, false);
#endif
	display.setRotation(0);

	// Print Test Screen
	display.clearBuffer();
	display.setTextWrap(true);
	display.setTextColor(EPD_BLACK);
	display.setCursor(1, 1);
	display.setTextSize(2);
	display.println("05/05/2020 01:58");
	display.drawLine(0, 20, 250, 20, EPD_BLACK);
	display.setCursor(1, 26);
	display.setTextSize(3);
	display.print("Temp 21.56 C");
	display.setCursor(1, 54);
	display.print("RH   70.25 %");
	display.setCursor(1, 82);
	display.print("Bat  12.52 V");
	display.drawLine(0, 109, 250, 109, EPD_BLACK);
	display.setCursor(1, 113);
	display.setTextSize(1);
	display.print("Gh Climate Box Compartment 2\n");

	display.display();
	
}






//////////////////////////////////////////////////////////////////////////
// the loop function runs over and over again until power down or reset //
//////////////////////////////////////////////////////////////////////////
void loop() {

	////// If valid time mesage is available, updtae RTC time
	if (Serial.available()) {
		processSyncMessage();
	}


	////// Update and Read Date and Time from RTC
	DateTime now = rtc.now();
	s = int(now.second());
	m = int(now.minute());
	h = int(now.hour());
	dy = int(now.day());
	mo = int(now.month());
	yr = int(now.year());


	////// Status LED Blink
	LEDtime = millis();
	if (LEDtime - lastBlink >= BlinkTime) {
		if (LEDstate == LOW) {
			LEDstate = HIGH;
		}
		else {
			LEDstate = LOW;
		}
		digitalWrite(Debug_LED, LEDstate);
		lastBlink = LEDtime;


		///// Debug
		Serial.println("\n\n\n");
		Serial.print(now.year(), DEC);
		Serial.print('/');
		Serial.print(now.month(), DEC);
		Serial.print('/');
		Serial.print(now.day(), DEC);
		Serial.print(" (");
		Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
		Serial.print(") ");
		Serial.print(now.hour(), DEC);
		Serial.print(':');
		Serial.print(now.minute(), DEC);
		Serial.print(':');
		Serial.print(now.second(), DEC);
		Serial.println();

		Serial.print("\nADC Avg Temp: ");
		Serial.println(adcTemp, 4);
		Serial.print("Battery Voltage: ");
		sum = 0;
		for (int i = 0; i < n; i++) {
			sum += analogRead(Battery_PIN) * VRef / 4095 * ((3.296 + 0.993) / 0.993);
		}
		BatteryV = sum / n;
		Serial.println(BatteryV, 4);
		Serial.print("AC_PIN: ");
		sum = 0;
		for (int i = 0; i < n; i++) {
			sum += (analogRead(AC_PIN) * VRef / 4095);
		}
		Serial.println(sum / n, 4);

		Serial.print("\nFan Setpoint: ");
		Serial.print(PID_Setpoint);
		Serial.println(" mV");
		Serial.print("Sense R avg voltage drop: ");
		Serial.print(PID_Input, 4);
		Serial.println(" mV");
		Serial.print("PID output: ");
		Serial.print(PID_Output, 4);
		Serial.println(" V");
		

	}
	
	
	////// PID fan speed control
	sum = 0;
	for (int i = 0; i < n; i++) {
		sum += analogRead(FanI_PIN) * VRef / 4095 * 1000;
	}
	PID_Input = sum / n;
	fanPID.Compute();
	analogWrite(A0, (PID_Output * 1023) / VRef);
	

	////// Test if it is time to Read sensors and Sum for later average
	if (m != LastSum) {
		
		////// Read Temp and RH
		getTempRH(); // Reads from T9602 sesnor. It uses two custum functions defined in attached .ino file.
		
		////// Read ADC
		// Based on https://github.com/annem/AD7193/blob/master/examples/AD7193_VoltageMeasure_Example/AD7193_VoltageMeasure_Example.ino
		//ch1Data = (AD7193.ReadADCChannel(1) >> 8); // Read channel measurement data
		//ch1Voltage = AD7193.DataToVoltage(ch1Data); // Convert to voltage
		//GlobalR = ch1Voltage * multiplier; // Convert to watts per square meter
		chTempData = (AD7193.ReadADCChannel(8) >> 8); // Read temperature data from chip registers
		adcTemp = AD7193.TempSensorDataToDegC(chTempData); // Convert temperature data to degC
		
		////// Add new values to sum
		TempSum += Temp; // "+=" is a compound addition. x = x + y is equivalent to x += y
		RHSum += RH;
		//GlobalRSum += GlobalR;
		adcTempSum += adcTemp;

		//// Update Shift registers
		LastSum = m;
		SumNum += 1;
	}
	

	////// Test if it is time to Update e-ink display and Log to SD card
	if (((m % 5) == 0) && (m != LastLog)) {


		////// Calculate Average values
		TempAvg = TempSum / SumNum;
		RHAvg = RHSum / SumNum;
		//GlobalRAvg = GlobalRSum / SumNum;
		adcTempAvg = adcTempSum / SumNum;
		

		////// Read AC power state and Battery voltage
		// Expected voltages at AC_Pin are 0.0 and 2.8 when AC is OK and OFF, respectiveley
		sum = 0;
		for (int i = 0; i < n; i++) {
			sum += (analogRead(AC_PIN) * VRef / 4095);
		}
		if ((sum / n) < 1.5) {
			ACok = 1;
		}
		else{
			ACok = 0;
		}
		

		// A voltage divider is used to read battery voltage, r1 = 3.296 Mohms r2 = 0.993 Mohms
		sum = 0;
		for (int i = 0; i < n; i++) {
			sum += analogRead(Battery_PIN) * VRef / 4095 * ((3.296 + 0.993) / 0.993);
		}
		BatteryV = sum / n;

		////// Open Year LogFile (create if not available)
		FileName = (String) yr + ".txt";
		if (!SD.exists(FileName)) {
			LogFile = SD.open(FileName, FILE_WRITE); // Create file
			
			// Add Metadata
			LogFile.println((String) "Station Number\t" + StaNum);
			LogFile.println((String) "Station Name\t" + StaName);
			LogFile.println((String) "Station Type\t" + StaType);
			LogFile.println((String) "Firmware\t" + Firmware);
			LogFile.println((String) "M0 Voltage Reference\t" + VRef);
			LogFile.println("");
			LogFile.println("");
			LogFile.println("");
			LogFile.println("");
			LogFile.println("");
			LogFile.println("");
			LogFile.println("");

			LogFile.println(Headers); // Add Headers
		}
		else {
			LogFile = SD.open(FileName, FILE_WRITE); // Open file
		}


		////// Log to SD card
		LogString = (String) yr + "\t" + mo + "\t" + dy + "\t" + h + "\t" + m + "\t" + s + "\t" +
			String(TempAvg, 4) + "\t" + String(RHAvg, 4) + "\t" + String(adcTempAvg, 4) + "\t" +
			String(PID_Input, 4) + "\t" + ACok + "\t" + String(BatteryV, 4) + "\t" + "0";
		LogFile.println(LogString); // Prints Log string to SD card file "LogFile.txt"
		LogFile.close(); // Close SD card file


		////// Update e-ink display
		display.clearBuffer();
		
		display.setCursor(1, 1);
		display.setTextSize(2);
		if (m <= 9) {
			display.println((String)dy + "/" + mo + "/" + yr + " " + h + ":0" + m);
		}
		else {
			display.println((String)dy + "/" + mo + "/" + yr + " " + h + ":" + m);
		}
		display.drawLine(0, 20, 250, 20, EPD_BLACK);
		display.setCursor(1, 26);
		display.setTextSize(3);
		display.print((String) "Temp " + TempAvg + " C");
		display.setCursor(1, 54);
		display.print((String) "RH   " + RHAvg + " %");
		display.setCursor(1, 82);
		display.print((String) "Bat  " + BatteryV + " V");
		display.drawLine(0, 109, 250, 109, EPD_BLACK);
		display.setCursor(1, 113);
		display.setTextSize(1);
		display.print(StaName);

		display.display();


		////// Reset Shift Registers
		LastLog = m;
		TempSum = 0;
		RHSum = 0;
		//GlobalRSum = 0;
		adcTempSum = 0;
		SumNum = 0;
		LogRdy = 1;
	}

}
