/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

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



#define EPD_CS     9 // verde
#define EPD_DC      10 // amarillo
#define SRAM_CS     6
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)

 /* Uncomment the following line if you are using 1.54" tricolor EPD */
 //Adafruit_IL0373 display(152, 152, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

 /* Uncomment the following line if you are using 1.54" monochrome EPD */
 //Adafruit_SSD1608 display(200, 200, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

 /* Uncomment the following line if you are using 2.13" tricolor EPD */
//Adafruit_IL0373 display(212, 104, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
//#define FLEXIBLE_213

/* Uncomment the following line if you are using 2.13" monochrome 250*122 EPD */
Adafruit_SSD1675 display(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.7" tricolor or grayscale EPD */
//Adafruit_IL91874 display(264, 176, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS);

/* Uncomment the following line if you are using 2.9" EPD */
//Adafruit_IL0373 display(296, 128, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
//#define FLEXIBLE_290

/* Uncomment the following line if you are using 4.2" tricolor EPD */
//Adafruit_IL0398 display(300, 400, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);


#define COLOR1 EPD_BLACK
#define COLOR2 EPD_RED



/////////
// ADC //
/////////
#include <AD7193_CS11.h> // Based on original AD7193.h library by Anne Mahaffey with pin 11 as CS
#include <SPI.h>
AD7193 AD7193;
unsigned long chTempData;
byte ADC_Status_ChNumber;
float adcTemp = -1; // ADC chip internal temperature




void setup(void) {
	Serial.begin(9600);
	delay(100);

	

	Serial.begin(115200);
	Serial.print("Hello!");

	AD7193.begin();
	AD7193.Reset();
	AD7193.AppendStatusValuetoData();
	AD7193.Calibrate(); // Calibrate with given PGA settings - need to recalibrate if PGA setting is changed
	
	digitalWrite(12, HIGH);

	chTempData = (AD7193.ReadADCChannel(8) >> 8); // Read temperature data from chip registers
	adcTemp = AD7193.TempSensorDataToDegC(chTempData); // Convert temperature data to degC
	Serial.println((String)"Tempdata: " + chTempData);
	Serial.println((String)"Temp ADC: " + adcTemp);
	Serial.println(chTempData, HEX);




	

	display.begin();
#if defined(FLEXIBLE_213) || defined(FLEXIBLE_290)
	// The flexible displays have different buffers and invert settings!
	display.setBlackBuffer(1, false);
	display.setColorBuffer(1, false);
#endif

	Serial.println("Initialized");

	display.setRotation(0);

	// large block of text
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

	
	chTempData = (AD7193.ReadADCChannel(8) >> 8); // Read temperature data from chip registers
	adcTemp = AD7193.TempSensorDataToDegC(chTempData); // Convert temperature data to degC
	Serial.println((String)"Tempdata 2: " + chTempData);
	Serial.println((String)"Temp ADC 2: " + adcTemp);
	Serial.println(chTempData, HEX);

	delay(5000);

	display.clearBuffer();
	display.setTextWrap(true);
	display.setTextColor(EPD_BLACK);
	display.setTextSize(4);
	display.setCursor(1, 54);
	display.print("WOW!!! ;-)");
	display.display();


}

void loop() {
	delay(500);
}