#include "Arduino.h"

constexpr const int pwm_pin = 9;
constexpr const int pll_ratio = 16;

constexpr int pwm_freq(double target_mhz) {
	return static_cast<int>(target_mhz*1e6/pll_ratio);
}

void setup() {
	pinMode(pwm_pin, OUTPUT);
	analogWriteFrequency(pwm_pin, pwm_freq(14.000 - 0.005));
	analogWrite(pwm_pin, 0x7F);
}

void loop() {
	delay(1000);
}
