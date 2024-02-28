#include <Arduino.h>
#include <SPI.h>

#include <cstdint>

constexpr const int select_pin = 10;

void deselect() {
	digitalWrite(select_pin, HIGH);
}

void select() {
	digitalWrite(select_pin, LOW);
}

void setup() {
	pinMode(select_pin, OUTPUT);
	deselect();
	SPI.begin();
	SPI.beginTransaction(SPISettings(20e6, MSBFIRST, SPI_MODE0));

	Serial.begin(115200);
}

const size_t buffer_length = 2*2;
uint8_t buffer[buffer_length];

void loop() {
	SPI.transfer(buffer, buffer_length);
	for (size_t i = 0; i < buffer_length; ++i) {
		buffer[i] = i & 1;
	}
}
