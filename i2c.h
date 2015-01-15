#ifndef I2C_H
#define I2C_H

template<const int INSTANCE,
	const bool MASTER = true,
	const long FREQUENCY = 100000>
struct I2C_T {
	static void init(void) {
	}

	static void enable(void) {
	}

	static void disable(void) {
	}

	static void set_slave_addr(uint8_t addr) {
	}

	static void write(const uint8_t *data, uint32_t length) {
	}

	static void read(uint8_t *data, uint32_t length) {
	}

	static void write_reg(const uint8_t reg, const uint8_t value) {
	}

	static uint8_t read_reg(const uint8_t reg) {
	}

	static void read_reg(const uint8_t reg, uint8_t *data, uint32_t length) {
	}
};

#endif
