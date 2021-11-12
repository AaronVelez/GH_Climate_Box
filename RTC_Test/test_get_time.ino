#include <DateTime.h>

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  255   // Header tag for serial time sync message

void setup() {
    Serial.begin(19200);
}

void  loop() {
    getPCtime(); // try to get time from the PC
    delay(100);
}

boolean getPCtime() {
    // if time sync available from serial port, update time and return true
    while (Serial.available() >= TIME_MSG_LEN) {  // time message consists of a header and ten ascii digits
        if (Serial.read() == TIME_HEADER) {
            time_t pctime = 0;
            for (int i = 0; i < TIME_MSG_LEN - 1; i++) {
                char c = Serial.read();
                if (c >= '0' && c <= '9') {
                    pctime = (10 * pctime) + (c - '0'); // convert digits to a number    
                }
            }
            setTime(pctime);   // Sync Arduino clock to the time received on the serial port
            return true;   // return true if time message received on the serial port
        }
    }
    return false;  //if no message return false
}

void setTime(time_t time)
{
    byte Second, Minute, Hour, Day, DayofWeek, Month, Year; // note year is actual year minus 2000 (ie 9 is 2009)
    DateTime.localTime(&time, &Second, &Minute, &Hour, &Day, &DayofWeek, &Month, &Year);
    // do something with the values to set the RTC here;
    Serial.println(Hour, DEC);
    Serial.println(Minute, DEC);
    Serial.println(Second, DEC);
    Serial.println();
}

