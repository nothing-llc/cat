/*
 * main data acquisition for the radio. partially stolen from the spi_dma
 * example in the pico-examples repo.
 * 
 * copyright (c) 2024  catherine van west <catherine.vanwest@cooper.edu>
 */
#include "hardware/dma.h"
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

// fill a buffer with the 0x0001 0x0001 ... pattern necessary for fast
// re-enabling of the adc
void fill_for_tx(uint8_t* buffer, const size_t length) {
	for (size_t i = 0; i < length; ++i) {
		buffer[i] = i & 1;
	}
}

int main() {
	annotate_program();
	stdio_init_all();

	// set up spi pins
	spi_init(spi_default, 20e6);
	gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

	constexpr size_t buffer_length = 1024;
	uint8_t tx_buffer[buffer_length];
	uint8_t rx_buffer[buffer_length];
	fill_for_tx(tx_buffer, buffer_length);

	// set up for dma; these sections are verbatim from the example
	const int tx_channel = dma_claim_unused_channel(true);
	const int rx_channel = dma_claim_unused_channel(true);

	// set up the outbound dma to transfer from the tx buffer to the spi
	// transmit fifo, paced by the spi tx dreq
	dma_channel_config tx_config =
		dma_channel_get_default_config(tx_channel);
	channel_config_set_transfer_data_size(&tx_config, DMA_SIZE_8);

	// set up dreq for the transmit side (hence true)
	channel_config_set_dreq(&tx_config, spi_get_dreq(spi_default, true));
	dma_channel_configure(
		tx_channel, &tx_config,
		&spi_get_hw(spi_default)->dr, // write address
		tx_buffer, // read address
		buffer_length, // element count
		false // don't start yet
	);

	dma_channel_config rx_config =
		dma_channel_get_default_config(rx_channel);
	channel_config_set_transfer_data_size(&rx_config, DMA_SIZE_8);

	// set up dreq for the receive side (hence false)
	channel_config_set_dreq(&rx_config, spi_get_dreq(spi_default, false));

	// set the channel to increment addresses only on the memory side, not
	// on the spi side
	channel_config_set_read_increment(&rx_config, false);
	channel_config_set_write_increment(&rx_config, true);

	dma_channel_configure(
		rx_channel, &rx_config,
		rx_buffer,
		&spi_get_hw(spi_default)->dr, // read address
		buffer_length, // element count
		false // don't start yet
	);

	while (true) {
		dma_channel_configure(
			tx_channel, &tx_config,
			&spi_get_hw(spi_default)->dr, // write address
			tx_buffer, // read address
			buffer_length, // element count
			false // don't start yet
		);

		dma_channel_configure(
			rx_channel, &rx_config,
			rx_buffer,
			&spi_get_hw(spi_default)->dr, // read address
			buffer_length, // element count
			false // don't start yet
		);

		// start both channels simultaneously
		dma_start_channel_mask(
			(1u << tx_channel) | (1u << rx_channel)
		);
		dma_channel_wait_for_finish_blocking(rx_channel);

		double sum = 0;
		for (size_t i = 0; i < buffer_length - 1; i += 2) {
			sum += (rx_buffer[i] << 4) + (rx_buffer[i + 1] >> 4);
		}
		double average = sum / (buffer_length/2) * (3.3/0xff);
		printf("\e[G\e[Kaverage voltage: %0.2f V", average);

		sleep_ms(100);
	}

	dma_channel_unclaim(tx_channel);
	dma_channel_unclaim(rx_channel);
}
