#include "reference_freq.h"

#include "hardware/pwm.h"

constexpr const static float clock_speed_mhz = 125;
constexpr const static int pll_divider = 16;

static uint slice;
static pwm_config config;
static uint8_t integer;
static uint8_t fract;

constexpr static inline float calc_div(float freq) {
	/* (clock speed) / (freq) gets the ratio of how fast we're running to
	   how fast we want to; multiplying by (pll divider) corrects for the
	   pll; dividing by 2 corrects for the implicit divide-by-2 in the pll
	   counter. */
	return clock_speed_mhz * pll_divider / (freq * 2);
}

constexpr static inline float calc_freq(float div) {
	// the same in reverse, interestingly.
	return calc_div(div);
}

namespace reference_freq {

void mostly_configure() {
	gpio_set_function(pin, GPIO_FUNC_PWM);
	slice = pwm_gpio_to_slice_num(pin);
	config = pwm_get_default_config();
	pwm_config_set_wrap(&config, 1);
//	set_mhz(14.000f);
}

void enable() {
	pwm_set_enabled(slice, true);
	pwm_set_gpio_level(pin, 1);
}

void disable() {
	pwm_set_enabled(slice, false);
}

void set_mhz(float freq, bool start) {
	pwm_config_set_clkdiv(&config, calc_div(freq));
	pwm_init(slice, &config, false);
	if (start) enable();
}

float actual_freq(float freq) {
	float div = calc_div(freq);

	// clip to the nearest sixteenth
	float actual_div = static_cast<float>(
		static_cast<long int>(div*16)/16.0f
	);
	return calc_freq(actual_div);
}

} /* ~reference_freq:: */
