#include "WProgram.h"
#include "Wire.h"

#include "i2c_expander.h"

io_expander expander;

void setup()
{
	Wire.begin();
	Serial.begin(57600);
	pinMode(2, INPUT);
	
	init_io_expander(&expander, 2);
	
	// RB1 is output.
	pinMode(&expander, RB1, OUTPUT);
	
	// Interrupt whenever RC1 changes.
	attachInterrupt(&expander, RC1);
}

void loop()
{
	// Whenever RC1 changes, copy it to RB1.
	if (digitalRead(2))
		digitalWrite(&expander, RB1, digitalRead(&expander, RC1));
	
	// Read analogue pin 0, printing to serial.
	Serial.println(analogRead(&expander, AN0), DEC);
}
