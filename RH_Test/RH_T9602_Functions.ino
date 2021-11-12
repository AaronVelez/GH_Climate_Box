

////// FUNCTIONS

/*
getTempRH calls readTempRH

readTempRH conects to I2C deivice and reads from registers

getTempRH gets the bytes values from readTempRH function and translates that into Temp and RH values
*/


// Function to read the RH and Temp data from sensor registers and write the values to state machine shift registers 
void getTempRH() {
	byte aa, bb, cc, dd;

	readTempRH(&aa, &bb, &cc, &dd); // Call function to read the data from sensor registers

  // humidity = (rH_High [5:0] x 256 + rH_Low [7:0]) / 16384 x 100
	RH = (float)(((aa & 0x3F) << 8) + bb) / 16384.0 * 100.0;

	// temperature = (Temp_High [7:0] x 64 + Temp_Low [7:2]/4 ) / 16384 x 165 - 40
	Temp = (float)((unsigned)(cc * 64) + (unsigned)(dd >> 2)) / 16384.0 * 165.0 - 40.0;

	Serial.print(Temp); Serial.print(" DegC  "); Serial.print(RH); Serial.println(" %RH");;
}

// Function to Read Temp and RH from T9602 Sensor. Addapted from https://github.com/AmphenolAdvancedSensors/Telaire/blob/T9602-ChipCap2/T9602.ino
void readTempRH(byte* a, byte* b, byte* c, byte* d) {
	Wire.requestFrom(0x28, 4); // Default I2C address
	*a = Wire.read();
	*b = Wire.read();
	*c = Wire.read();
	*d = Wire.read();
}