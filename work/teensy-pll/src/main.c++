#include "Arduino.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define PWM_PIN LED_BUILTIN

constexpr const int pll_ratio = 32;

constexpr int pwm_freq(double target_mhz) {
	return static_cast<int>(target_mhz*1e6/pll_ratio);
}

void setup() {
	pinMode(PWM_PIN, OUTPUT);
	analogWriteFrequency(13, pwm_freq(14.000));
	analogWrite(PWM_PIN, 0x7F);
}

void loop() {
	delay(1000);
}
