#include "Arduino.h"

void setup() {
	Serial.begin(115200);
}

void loop() {
	Serial.println("call me?");
	delay(1000);
}
