/***
	servo.pde:	Bitlash Servo User Function Code

	Copyright (C) 2008-2012 Bill Roy

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:
	
	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

***/
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include "bitlash.h"
#include "Chatpad.h"
#include <AltSoftSerial.h>


//#include "Servo.h"
#include "ServoTimer2.h"

#define NUMSERVOS 6			// bump this for more per http://www.arduino.cc/en/Reference/Servo

byte servo_install_count;	// number of servos that have been initialized
byte servo_pin[NUMSERVOS];	// the pins of those servos
ServoTimer2 myservos[NUMSERVOS];	// the Servo object for those servos
Chatpad chatpad;// Create a Chatpad object.
AltSoftSerial altSerial;

//////////
// servohandler: function handler for Bitlash servo() function
//
//	arg 1: servopin
//	arg 2: setting
//
numvar servohandler(void) {
byte slot = 0;
	// is this pin already allocated a servo slot?
	while (slot < servo_install_count) {
		if (servo_pin[slot] == getarg(1)) break;
		slot++;
	}
	if (slot >= servo_install_count) {	// not found

		// do we have a free slot to allocate?
		if (servo_install_count < NUMSERVOS) {
			slot = servo_install_count++;
			servo_pin[slot] = getarg(1);
			myservos[slot].attach(getarg(1));
		}
		else {
			Serial.println("servo handler: ERROR!");	// please increase NUMSERVOS above
			return -1;
		}
	}
	myservos[slot].write(getarg(2));
	return 0;
}

void setup(void) {
  altSerial.begin(19200); // always 19200 for the chatpadSerial, cannot be moved into chatpad class since that takes a stream object and all streams shall be supported
  chatpad.Init(altSerial, onPress,nothing);
	initBitlash(57600);		// must be first to initialize serial port

	// Register the extension function with Bitlash:
	// 		"servo" is the name Bitlash will match for the function
	// 		(bitlash_function) servohandler is the C function handler declared above
	//
	addBitlashFunction("servo", (bitlash_function) servohandler);
  addBitlashFunction("clear", (bitlash_function) clearHandler);
}

void clearHandler(){
  Serial.write(27);       // ESC command
  Serial.print("[2J");    // clear screen command
  Serial.write(27);
  Serial.print("[H");     // cursor to home command
}

void nothing(char key){}
void onPress(char key){
  doCharacter(key);
}

void loop(void) {
	runBitlash();
  chatpad.run();
}
