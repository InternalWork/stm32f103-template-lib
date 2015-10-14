#ifndef I2C_H
#define I2C_H

#include <stm32f10x.h>
#include <clocks.h>

namespace I2C {

I2C_TypeDef I2C1_P_ __attribute__((section(".i2c1_peripheral")));
I2C_TypeDef * const I2C1_P = (I2C_TypeDef *) &I2C1_P_;

I2C_TypeDef I2C_P_ __attribute__((section(".i2c2_peripheral")));
I2C_TypeDef * const I2C_P = (I2C_TypeDef *) &I2C_P_;

static constexpr I2C_TypeDef *I2C_MODULE[2] = {I2C1_P, I2C_P};

template<typename SYSCLK,
	const int INSTANCE,
	const bool MASTER = true,
	const long FREQUENCY = 100000,
	const bool CLOCK_STRETCHING = false,
	const uint8_t OWN_ADDRESS1 = 0,
	const uint8_t OWN_ADDRESS2 = 0>
struct T {
	static uint8_t slave_addr;
	static constexpr I2C_TypeDef *I2C = I2C_MODULE[INSTANCE - 1];

	static void init(void) {
		I2C->CR1 &= ~I2C_CR1_PE;
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
		switch (INSTANCE) {
		case 1:
			RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
			RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
			RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
			break;
		case 2:
			RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
			RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
			RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST;
			break;
		}
		I2C->CR2 |= SYSCLK::pclk1 / 1000000;
		if (FREQUENCY <= 100000) {
			I2C->CCR = SYSCLK::pclk1 / (FREQUENCY * 2);
			I2C->TRISE = SYSCLK::pclk1 / 1000000 + 1;
		} else {
			I2C->CCR = SYSCLK::pclk1 / (FREQUENCY * 3) | I2C_CCR_FS;
			I2C->TRISE = (SYSCLK::pclk1 / 1000000) * 3 / 10 + 1;
		}
		if (!MASTER) {
			I2C->OAR1 = (OWN_ADDRESS1 << 1) | 0x4000;
			if (OWN_ADDRESS2) {
				I2C->OAR2 = (OWN_ADDRESS2 << 1) | I2C_OAR2_ENDUAL;
			}
		}
		I2C->CR1 |= I2C_CR1_PE | I2C_CR1_ACK;
	}

	static void enable(void) {
		switch (INSTANCE) {
		case 1: RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; break;
		case 2: RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; break;
		}
	}

	static void disable(void) {
		switch (INSTANCE) {
		case 1: RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN; break;
		case 2: RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN; break;
		}
	}

	static void set_slave_addr(uint8_t addr) {
		slave_addr = addr;
	}

	static void write(const uint8_t *data, uint32_t length) {
	}

	static void read(uint8_t *data, uint32_t length) {
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static bool generate_start(void) {
		I2C->CR1 |= I2C_CR1_START;
		while (!TIMEOUT::triggered() && !(I2C->SR1 & I2C_SR1_SB));
		return !(TIMEOUT::triggered());
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static bool send_slave_addr(uint8_t slave_addr) {
		I2C->DR = slave_addr;
		while (!TIMEOUT::triggered() && !(I2C->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)));
		return !(TIMEOUT::triggered() || I2C->SR1 & I2C_SR1_AF);
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static bool wait_for_txe(void) {
		while ((CLOCK_STRETCHING && !TIMEOUT::triggered()) && !(I2C->SR1 & I2C_SR1_TXE));
		return !(CLOCK_STRETCHING && TIMEOUT::triggered());
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static bool wait_for_stop(void) {
		while (!TIMEOUT::triggered() && !(I2C->CR1 & I2C_CR1_STOP));
		return !(TIMEOUT::triggered());
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static bool write_reg(const uint8_t reg, const uint8_t value) {
		uint8_t r;

		if (!generate_start<TIMEOUT>()) goto timeout;
		if (!send_slave_addr<TIMEOUT>(slave_addr << 1)) goto timeout;
		r = I2C->SR2;
		if (!wait_for_txe<TIMEOUT>()) goto timeout;
		I2C->DR = reg;
		if (!wait_for_txe<TIMEOUT>()) goto timeout;
		I2C->DR = value;
		if (!wait_for_txe<TIMEOUT>()) goto timeout;
		I2C->CR1 |= I2C_CR1_STOP;
		if (!wait_for_stop<TIMEOUT>()) goto timeout;
		return true;
	timeout:
		return false;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static bool read_reg(const uint8_t reg, uint8_t& value) {
		uint8_t r = 0;

		if (!generate_start<TIMEOUT>()) goto timeout;
		if (!send_slave_addr<TIMEOUT>(slave_addr << 1)) goto timeout;
		r = I2C->SR2;
		if (!wait_for_txe<TIMEOUT>()) goto timeout;
		I2C->DR = reg;
		if (!wait_for_txe<TIMEOUT>()) goto timeout;
		if (!generate_start<TIMEOUT>()) goto timeout;
		if (!send_slave_addr<TIMEOUT>((slave_addr << 1) | 1)) goto timeout;
		I2C->CR1 &= ~I2C_CR1_ACK;
		r = I2C->SR2;
		I2C->CR1 |= I2C_CR1_STOP;
		if (!wait_for_txe<TIMEOUT>()) goto timeout;
		value = I2C->DR;
		if (!wait_for_stop<TIMEOUT>()) goto timeout;
		I2C->CR1 |= I2C_CR1_ACK;
		return true;
	timeout:
		return false;
	}

	template<typename TIMEOUT = TIMEOUT_NEVER>
	static void read_reg(const uint8_t reg, uint8_t *data, uint32_t length) {
		uint8_t r;

		if (!generate_start<TIMEOUT>()) goto timeout;
		if (!send_slave_addr<TIMEOUT>(slave_addr << 1)) goto timeout;
		r = I2C->SR2;
		if (!wait_for_txe<TIMEOUT>()) goto timeout;
		I2C->DR = reg;
		if (!wait_for_txe<TIMEOUT>()) goto timeout;

		if (!generate_start<TIMEOUT>()) goto timeout;
		if (!send_slave_addr<TIMEOUT>((slave_addr << 1) | 1)) goto timeout;
		r = I2C->SR2;
		while (length > 0) {
			if (length != 3) {
				*data++ = I2C->DR;
				length--;
			}
			if (length == 3) {
				while (!TIMEOUT::triggered() && !(I2C->SR1 & I2C_SR1_BTF)); if (TIMEOUT::triggered()) goto timeout;
				I2C->CR1 &= ~I2C_CR1_ACK;
				*data++ = I2C->DR;
				I2C->CR1 |= I2C_CR1_STOP;
				*data++ = I2C->DR;
				while (!TIMEOUT::triggered() && !(I2C->SR1 & I2C_SR1_RXNE)); if (TIMEOUT::triggered()) goto timeout;
				*data++ = I2C->DR;
				length = 0;
			}
		}
		if (!wait_for_stop<TIMEOUT>()) goto timeout;
		I2C->CR1 |= I2C_CR1_ACK;
	timeout:
		return;
	}

	static bool addr_matched(void) {
		return I2C->SR1 & I2C_SR1_ADDR;
	}

	static void clear_addr_matched(void) {
		volatile uint32_t temp;
		temp = I2C->SR1;
		temp = I2C->SR2;
	}
};

template<typename SYSCLK, const int INSTANCE, const bool MASTER, const long FREQUENCY, const bool CLOCK_STRETCHING,
	const uint8_t OWN_ADDRESS1, const uint8_t OWN_ADDRESS2>
uint8_t T<SYSCLK, INSTANCE, MASTER, FREQUENCY, CLOCK_STRETCHING, OWN_ADDRESS1, OWN_ADDRESS2>::slave_addr;

}

#endif
