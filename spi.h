#ifndef __SPI_H
#define __SPI_H

#include <stm32f10x.h>
#include <tasks.h>
#include <clocks.h>
#include <utils.h>

enum SPI_INSTANCE {
	SPI_1 = 0,
	SPI_2,
#ifdef RCC_APB1ENR_SPI3EN
	SPI_3
#endif
};

static constexpr SPI_TypeDef *spi_addr[] = {
	SPI1,
	SPI2
#ifdef RCC_APB1ENR_SPI3EN
	SPI3
#endif
};

static constexpr uint32_t rcc_spi_en[] = {
	RCC_APB2ENR_SPI1EN, RCC_APB1ENR_SPI2EN,
#ifdef RCC_APB1ENR_SPI3EN
	RCC_APB1ENR_SPI3EN
#endif
};

static constexpr IRQn_Type nvic_spi_irqn[] = {
	SPI1_IRQn, SPI2_IRQn,
#ifdef RCC_APB1ENR_SPI3EN
	SPI1_IRQn
#endif
};

template<const SPI_INSTANCE SPI_N,
	typename CLOCK,
	const bool MASTER = true,
	const int MODE = 3,
	const int SPEED = 1000000,
	const int DATA_LENGTH = 8,
	const bool MSB = true>
struct SPI_T {
	static constexpr SPI_TypeDef *spi = spi_addr[SPI_N];
	volatile static int tx_count;
	volatile static int rx_count;
	static uint8_t *rx_buffer;
	static uint8_t *tx_buffer;


	static constexpr int baudrate(const int rate, const int base_clock) {
		return (rate >= base_clock / 2) ? 0 :
			(rate >= base_clock / 4) ? 1 :
			(rate >= base_clock / 8) ? 2 :
			(rate >= base_clock / 16) ? 3 :
			(rate >= base_clock / 32) ? 4 :
			(rate >= base_clock / 64) ? 5 :
			(rate >= base_clock / 128) ? 6 :
			(rate >= base_clock / 256) ? 7 : 7;
	}

	static void init(void) {
		(SPI_N == SPI_1 ? RCC->APB2ENR : RCC->APB1ENR) |= rcc_spi_en[SPI_N];

		spi->CR1 = ((MODE & 0x1) ? SPI_CR1_CPHA : 0) |
			((MODE & 0x2) ? SPI_CR1_CPOL : 0) |
			(MASTER ? SPI_CR1_MSTR : 0) |
			(MSB ? 0 : SPI_CR1_LSBFIRST) |
			baudrate(SPEED, CLOCK::frequency) << 3 |
			(DATA_LENGTH == 16 ? SPI_CR1_DFF : 0) |
			SPI_CR1_SSM | SPI_CR1_SSI;

		if (spi->SR & SPI_SR_MODF) {
			spi->CR1 = spi->CR1;
		}
		spi->CR1 |= SPI_CR1_SPE;
		NVIC_EnableIRQ(nvic_spi_irqn[SPI_N]);
		NVIC_ClearPendingIRQ(nvic_spi_irqn[SPI_N]);
	}

	static void disable(void) {
		spi->CR1 &= ~SPI_CR1_SPE;
		(SPI_N == SPI_1 ? RCC->APB2ENR : RCC->APB1ENR) &= ~rcc_spi_en[SPI_N];
	}

	static void enable_irq(void) {
		spi->CR2 |= SPI_CR2_RXNEIE | SPI_CR2_TXEIE;
	}

	static void disable_irq(void) {
		spi->CR2 &= ~(SPI_CR2_RXNEIE | SPI_CR2_TXEIE);
	}

	static bool busy(void) {
		return spi->SR & SPI_SR_BSY;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static uint8_t transfer(uint8_t data) {
		while (!TIMEOUT::triggered() && !(spi->SR & SPI_SR_TXE));
		spi->DR = data;
		while (!TIMEOUT::triggered() && !(spi->SR & SPI_SR_RXNE));
		return spi->DR;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void transfer(uint8_t *tx_data, uint32_t count, uint8_t *rx_data = 0) {
		tx_buffer = tx_data;
		tx_count = count;
		rx_buffer = rx_data;
		rx_count = count;
		enable_irq();
		if (rx_count > 0) {
			while (!TIMEOUT::triggered() && rx_count > 0) {
				enter_idle();
			}
		}
		disable_irq();
	}

	static bool handle_irq(void) {
		bool resume = false;
		uint16_t status = spi->SR;

		if (status & SPI_SR_RXNE) {
			uint16_t received_data = spi->DR;
			if (rx_count > 0) {
				if (rx_buffer) {
					*rx_buffer = received_data;
					rx_buffer++;
				}
				rx_count--;
			} else {
				spi->CR2 &= ~SPI_CR2_RXNEIE;
				resume = true;
			}
		}
		if (status & SPI_SR_TXE) {
			if (tx_count > 0) {
				if (tx_buffer) {
					spi->DR = *tx_buffer;
					tx_buffer++;
				} else {
					spi->DR = 0xff;
				}
				tx_count--;
			} else {
				spi->CR2 &= ~SPI_CR2_TXEIE;
			}
		}
		return resume;
	}

};

template<const SPI_INSTANCE SPI_N, typename CLOCK, const bool MASTER, const int MODE, const int SPEED, const int DATA_LENGTH, const bool MSB>
volatile int SPI_T<SPI_N, CLOCK, MASTER, MODE, SPEED, DATA_LENGTH, MSB>::tx_count;
template<const SPI_INSTANCE SPI_N, typename CLOCK, const bool MASTER, const int MODE, const int SPEED, const int DATA_LENGTH, const bool MSB>
volatile int SPI_T<SPI_N, CLOCK, MASTER, MODE, SPEED, DATA_LENGTH, MSB>::rx_count;
template<const SPI_INSTANCE SPI_N, typename CLOCK, const bool MASTER, const int MODE, const int SPEED, const int DATA_LENGTH, const bool MSB>
uint8_t *SPI_T<SPI_N, CLOCK, MASTER, MODE, SPEED, DATA_LENGTH, MSB>::rx_buffer;
template<const SPI_INSTANCE SPI_N, typename CLOCK, const bool MASTER, const int MODE, const int SPEED, const int DATA_LENGTH, const bool MSB>
uint8_t *SPI_T<SPI_N, CLOCK, MASTER, MODE, SPEED, DATA_LENGTH, MSB>::tx_buffer;

#endif
