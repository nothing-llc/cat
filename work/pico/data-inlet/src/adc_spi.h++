/*
 * main data acquisition for the radio. partially stolen from the spi_dma
 * example in the pico-examples repo.
 * 
 * copyright (c) 2024  catherine van west <catherine.vanwest@cooper.edu>
 */
#include "hardware/dma.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <cstdint>
#include <array>

template <size_t buffer_length>
class adc_spi {
	static_assert(buffer_length % 2 == 0, "buffer length cannot be odd");

public:
	using buffer_type = std::array<uint8_t, buffer_length>;

	adc_spi(
		spi_inst_t* spi,
		uint baud,
		uint spi_tx_pin,
		uint spi_rx_pin, 
		uint spi_sck_pin
	);
	~adc_spi();

	inline uint8_t& operator[] (size_t pos) {
		return rx_buffer[pos];
	}

	// rewind both channels, then start reading/writing
	inline void start();

	// wait to finish reading
	inline void wait() {
		dma_channel_wait_for_finish_blocking(rx_channel);
	}

private:
	spi_inst_t* spi;
	unsigned int baud;

	// dma channels for spi acquisition
	int tx_channel;
	int rx_channel;
	dma_channel_config tx_config;
	dma_channel_config rx_config;
	
	buffer_type tx_buffer;
	buffer_type rx_buffer;

	inline void fill_for_tx() {
		for (size_t i = 0; i < buffer_length; ++i) {
			tx_buffer[i] = i & 1;
		}
	}

	inline void dma_channel_init(
		int& channel,
		dma_channel_config& config,
		buffer_type& buffer,
		bool is_tx
	);

	inline void rewind_tx();
	inline void rewind_rx();
};

template <size_t buffer_length>
adc_spi<buffer_length>::adc_spi(
	spi_inst_t* spi,
	uint baud,
	uint spi_tx_pin,
	uint spi_rx_pin,
	uint spi_sck_pin
) : spi(spi), baud(baud) {
	// set up spi pins
	spi_init(spi, baud);
	gpio_set_function(spi_tx_pin,  GPIO_FUNC_SPI);
	gpio_set_function(spi_rx_pin,  GPIO_FUNC_SPI);
	gpio_set_function(spi_sck_pin, GPIO_FUNC_SPI);

	// set up channels
	dma_channel_init(tx_channel, tx_config, tx_buffer, true);
	dma_channel_init(rx_channel, rx_config, rx_buffer, false);

	// prep the transmit buffer
	fill_for_tx();
}

template <size_t buffer_length>
adc_spi<buffer_length>::~adc_spi() {
	dma_channel_unclaim(tx_channel);
	dma_channel_unclaim(rx_channel);
}

template <size_t buffer_length>
void adc_spi<buffer_length>::start() {
	rewind_tx();
	rewind_rx();

	// start both channels simultaneously
	dma_start_channel_mask((1u << tx_channel) | (1u << rx_channel));
}

template <size_t buffer_length>
void adc_spi<buffer_length>::dma_channel_init(
	int& channel,
	dma_channel_config& config,
	buffer_type& buffer,
	bool is_tx
) {
	// much of this code is verbatim from the example
	channel = dma_claim_unused_channel(true);
	config = dma_channel_get_default_config(channel);
	channel_config_set_transfer_data_size(&config, DMA_SIZE_8);

	// set up to transfer from the buffer to/from spi paced by the
	// appropriate spi dreq
	channel_config_set_dreq(&config, spi_get_dreq(spi, is_tx));

	if (!is_tx) {
		// set the channel to increment addresses only on the memory
		// side, not on the spi side
		channel_config_set_read_increment(&config, false);
		channel_config_set_write_increment(&config, true);
	}

	// note: config is not written until we actually start!
}



template <size_t buffer_length>
inline void adc_spi<buffer_length>::rewind_tx() {
	dma_channel_configure(
		tx_channel, &tx_config,
		&spi_get_hw(spi)->dr, // write address
		tx_buffer.data(), // read address
		buffer_length, // element count
		false // don't start yet
	);
}

template <size_t buffer_length>
inline void adc_spi<buffer_length>::rewind_rx() {
	dma_channel_configure(
		rx_channel, &rx_config,
		rx_buffer.data(), // write address
		&spi_get_hw(spi)->dr, // read address
		buffer_length, // element count
		false // don't start yet
	);
}
