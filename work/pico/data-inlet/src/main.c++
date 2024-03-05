/*
 * main pico code. pretty self-explanatory, hopefully.
 * 
 * copyright (c) 2024  catherine van west <catherine.vanwest@cooper.edu>
 */
#include "adc_spi.h++"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
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

int main() {
	annotate_program();
	stdio_init_all();

	// get ready for spi
	const size_t buffer_length = 1024;
	const uint baud = 21e6; // 20.8 MHz spi clock ⇒ 1.0965 MHz sample rate
	adc_spi<buffer_length> adc(
		spi_default,
		baud,
		PICO_DEFAULT_SPI_TX_PIN,
		PICO_DEFAULT_SPI_RX_PIN,
		PICO_DEFAULT_SPI_SCK_PIN
	);

	while (true) {
		adc.start();

		long unsigned int sum = 0;
		for (size_t i = 0; i < buffer_length - 1; i += 2) {
			sum += (adc[i] << 4) + (adc[i + 1] >> 4);
		}
		volatile double average = sum / (buffer_length/2) * (3.3/0xff);
//		auto start = get_absolute_time();

		adc.wait(true);
//		auto free_time = absolute_time_diff_us(
//			start, get_absolute_time()
//		);
//		printf(
//			"\e[G\e[Kaverage voltage: %0.3f V; free time: %d us",
//			average, free_time
//		);
	}
}
