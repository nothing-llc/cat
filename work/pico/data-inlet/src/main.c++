/*
 * main data acquisition for the radio. partially stolen from the spi example
 * in the pico-examples repo.
 * 
 * copyright (c) 2024  catherine van west <catherine.vanwest@cooper.edu>
 */
#include "hardware/spi.h"
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
		PICO_DEFAULT_SPI_RX_PIN,
		PICO_DEFAULT_SPI_TX_PIN,
		PICO_DEFAULT_SPI_SCK_PIN,
		GPIO_FUNC_SPI
	));
}

int main() {
	annotate_program();
	stdio_init_all();

	spi_init(spi_default, 20e6);
	gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

	constexpr size_t buffer_length = 2;
	uint8_t tx_buffer[buffer_length] = {0, 1};
	uint8_t rx_buffer[buffer_length];

	while (1) {
		spi_write_read_blocking(
			spi_default, tx_buffer, rx_buffer, buffer_length
		);
		printf(
			"\e[G\e[Kread voltage from spi: %d",
			(rx_buffer[0] << 8) + rx_buffer[1]
		);
		sleep_ms(100);
	}
}
