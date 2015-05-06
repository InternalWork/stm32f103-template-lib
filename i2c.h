#ifndef I2C_H
#define I2C_H

#include <stm32f10x.h>

template<typename SYSCLK,
	const int INSTANCE,
	const bool MASTER = true,
	const long FREQUENCY = 100000>
struct I2C_T {
	static uint8_t slave_addr;

	static void init(void) {
		I2C2->CR1 &= ~I2C_CR1_PE;
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST;
		I2C2->CR1 |= I2C_CR1_ACK;
		I2C2->CR2 |= SYSCLK::pclk1 / 1000000 / 2;
		I2C2->CCR |= SYSCLK::pclk1 / (FREQUENCY << 1) / 2;
		I2C2->TRISE = SYSCLK::pclk1 / 1000000 + 1;
		I2C2->CR1 |= I2C_CR1_PE;
	}

	static void enable(void) {
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}

	static void disable(void) {
		RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN;
	}

	static void set_slave_addr(uint8_t addr) {
		slave_addr = addr;
	}

	static void write(const uint8_t *data, uint32_t length) {
	}

	static void read(uint8_t *data, uint32_t length) {
	}

	static void write_reg(const uint8_t reg, const uint8_t value) {
		uint8_t r;

		I2C2->CR1 |= I2C_CR1_START;
		while (!(I2C2->SR1 & I2C_SR1_SB));
		//I2C2->DR = (slave_addr << 1);
		I2C2->DR = (0x5e << 1);
		while (!(I2C2->SR1 & I2C_SR1_ADDR));
		r = I2C2->SR2;
		while (!(I2C2->SR1 & I2C_SR1_TXE));
		I2C2->DR = reg;
		while (!(I2C2->SR1 & I2C_SR1_TXE));
		I2C2->DR = value;
		while (!(I2C2->SR1 & I2C_SR1_TXE));
		I2C2->CR1 |= I2C_CR1_STOP;
		while (I2C2->CR1 & I2C_CR1_STOP);
	}

	static uint8_t read_reg(const uint8_t reg) {
		uint8_t r;

		I2C2->CR1 |= I2C_CR1_START;
		while (!(I2C2->SR1 & I2C_SR1_SB));
		I2C2->DR = (slave_addr << 1);
		while (!(I2C2->SR1 & I2C_SR1_ADDR));
		r = I2C2->SR2;
		while (!(I2C2->SR1 & I2C_SR1_TXE));
		I2C2->DR = reg;
		while (!(I2C2->SR1 & I2C_SR1_TXE));

		I2C2->CR1 |= I2C_CR1_START;
		while (!(I2C2->SR1 & I2C_SR1_SB));
		I2C2->DR = (slave_addr << 1);
		while (!(I2C2->SR1 & I2C_SR1_ADDR));
		I2C2->CR1 &= ~I2C_CR1_ACK;
		r = I2C2->SR2;
		I2C2->CR1 |= I2C_CR1_STOP;
		while (!(I2C2->SR1 & I2C_SR1_RXNE));
		r = I2C2->DR;
		while (I2C2->CR1 & I2C_CR1_STOP);
		I2C2->CR1 |= I2C_CR1_ACK;
		return r;
	}

	static void read_reg(const uint8_t reg, uint8_t *data, uint32_t length) {
		uint8_t r;

		I2C2->CR1 |= I2C_CR1_START;
		while (!(I2C2->SR1 & I2C_SR1_SB));
		I2C2->DR = (slave_addr << 1);
		while (!(I2C2->SR1 & I2C_SR1_ADDR));
		r = I2C2->SR2;
		while (!(I2C2->SR1 & I2C_SR1_TXE));
		I2C2->DR = reg;
		while (!(I2C2->SR1 & I2C_SR1_TXE));

		I2C2->CR1 |= I2C_CR1_START;
		while (!(I2C2->SR1 & I2C_SR1_SB));
		I2C2->DR = (slave_addr << 1);
		while (!(I2C2->SR1 & I2C_SR1_ADDR));
		r = I2C2->SR2;
		while (length > 0) {
			if (length != 3) {
				//while (!I2C2->SR1 & I2C_SR1_BTF);
				*data++ = I2C2->DR;
				length--;
			}
			if (length == 3) {
				//while (!I2C2->SR1 & I2C_SR1_BTF);
				I2C2->CR1 &= ~I2C_CR1_ACK;
				*data++ = I2C2->DR;
				I2C2->CR1 |= I2C_CR1_STOP;
				*data++ = I2C2->DR;
				while (!(I2C2->SR1 & I2C_SR1_RXNE));
				*data++ = I2C2->DR;
				length = 0;
			}
		}
		while (I2C2->CR1 & I2C_CR1_STOP);
		I2C2->CR1 |= I2C_CR1_ACK;
	}
};

template<typename SYSCLK, const int INSTANCE, const bool MASTER, const long FREQUENCY>
uint8_t I2C_T<SYSCLK, INSTANCE, MASTER, FREQUENCY>::slave_addr;

#endif
