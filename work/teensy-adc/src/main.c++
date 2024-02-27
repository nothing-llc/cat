#include <Arduino.h>
#include <SPI.h>

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

unsigned short counter = 0;
static_assert(sizeof(counter) == 2, "augh!");

void loop() {
	select();
	int received = SPI.transfer16(++counter);
	deselect();

	Serial.print("\e[G\e[Kcall me? received = ");
	Serial.print(received);
	delay(100);
}
