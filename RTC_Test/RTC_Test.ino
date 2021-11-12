/*
 Name:		RTC_Test.ino
 Created:	5/29/2020 2:05:26 PM
 Author:	aivel
 Harware:	https://www.adafruit.com/product/3028
 IC:		DS3231
*/



//   ///// LIBRARIES /////   //


/* Use TimeLib library to sync date and time.
Source: Serial 
Format: Mesage with T + Unix time. Example: 483642000 = 29/Apr/1985 17:00:00 GMT
*/

#include <TimeLib.h>
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

// RTC library
#include <RTClib.h>
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };








//   ///// SETUP /////   //

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9600);
    rtc.begin();
    
    
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    


    //Set function to call when date and time sync required
    setSyncProvider(requestSync);
}









//   ///// LOOP /////   //

// the loop function runs over and over again until power down or reset
void loop() {
    // If valid time mesage is available, updtae RTC time
    if (Serial.available()) {
        processSyncMessage();
    }


    DateTime now = rtc.now();

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

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    delay(1000);
    
}







