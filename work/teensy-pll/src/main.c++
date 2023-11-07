#include "Arduino.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	analogWriteFrequency(13, 440000);
}

void loop() {
	static char duty = 0;
	++duty;
	analogWrite(LED_BUILTIN, duty);
	delay(10);
}
