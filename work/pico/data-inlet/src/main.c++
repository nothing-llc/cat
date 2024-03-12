/*
 * main pico code. pretty self-explanatory, hopefully.
 *
 * copyright (c) 2024  catherine van west <catherine.vanwest@cooper.edu>
 */

#include "adc_spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "reference_freq.h"
#include <cmath>
#include <cstdint>
#include <cstdio>

void annotate_program() {
	bi_decl(bi_program_description("SPI ADC read & magic DSP"));
//	bi_decl(bi_4pins_with_names(
//		PICO_DEFAULT_SPI_SCK_PIN,
//		"SCK pin",
//		PICO_DEFAULT_SPI_TX_PIN,
//		"TX pin",
//		PICO_DEFAULT_SPI_RX_PIN,
//		"RX pin",
//		PICO_DEFAULT_SPI_CSN_PIN,
//		"CSN pin"
//	));
	bi_decl(bi_3pins_with_func(
		PICO_DEFAULT_SPI_TX_PIN,
		PICO_DEFAULT_SPI_RX_PIN,
		PICO_DEFAULT_SPI_SCK_PIN,
		GPIO_FUNC_SPI
	));
}

void adc_test() {
	// get ready for spi
	const size_t buffer_length = 8*1024;
	const uint baud = 21e6; // 20.8 MHz spi clock ⇒ 1.0965 MHz sample rate
	adc_spi<buffer_length> adc(
		spi_default,
		baud,
		PICO_DEFAULT_SPI_TX_PIN,
		PICO_DEFAULT_SPI_RX_PIN,
		PICO_DEFAULT_SPI_SCK_PIN
	);

	const uint8_t half_voltage = static_cast<uint8_t>(3.3/2 * 0xff/3.3);
	const double sampling_rate = 1.0965e6; // in Hz

	while (true) {
		adc.start();

		int64_t index_diff_sum = 0;
		size_t zero_crossings = 0;

		size_t last_index = 0;
		bool sign = std::signbit(adc[0] - half_voltage);
		for (size_t i = 1; i < buffer_length - 1; i += 2) {
			// watch for zero crossings and record them
			if (sign ^ std::signbit(adc[i] - half_voltage)) {
				sign = !sign;
				index_diff_sum += i - last_index;
				last_index = i;
				++zero_crossings;
			}
		}

		const double index_diff_average =
			static_cast<double>(index_diff_sum)/zero_crossings;
		const double freq_average =
			sampling_rate/(index_diff_average*2);

		auto start_of_free = get_absolute_time();

		adc.wait(true);
		int64_t free_time = absolute_time_diff_us(
			start_of_free, get_absolute_time()
		);
		printf(
			"\e[G\e[Kaverage freq: %0.3f kHz"
			"; free time: %ld us",
			freq_average/1e3, free_time
		);
	}
}

void pwm_test() {
	reference_freq::mostly_configure();
	reference_freq::set_mhz(14.000);

	float freq = 14.000;
	while (true) {
		freq += .01;
		if (freq > 14.350) freq = 14.000;

		reference_freq::set_mhz(freq);
		printf(
			"now running at %.3f (%.3f) MHz\n",
			freq, reference_freq::actual_freq(freq)
		);

		sleep_ms(10e3);
	}
}

int main() {
	annotate_program();
	stdio_init_all();

	adc_test();
//	pwm_test();

	for (;;) tight_loop_contents();
}
