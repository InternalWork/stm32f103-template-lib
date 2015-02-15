#include <clocks.h>
#include <gpio.h>
#include <i2c.h>

typedef SYSCLK_T<> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> D3;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, OUTPUT_10MHZ, PUSH_PULL> D5;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL> D6;
typedef GPIO_T<PA, 9, OUTPUT_10MHZ, PUSH_PULL> D7;
typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_PORT_T<PA, D0, D1, D2, D3, D6, D7> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON> PORT_C;

typedef I2C_T<1> I2C;

uint8_t chip_id;
uint8_t ctrl_meas;
uint8_t out[3];

int main(void)
{
	SYSCLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	I2C::init();
	I2C::set_slave_addr(0x77);
	while (1) {
		I2C::write_reg(0xf4, 0xc0);
		chip_id = I2C::read_reg(0xd0);
		ctrl_meas = I2C::read_reg(0xf4);
		I2C::read_reg(0xf6, out, sizeof(out));
		SYSTICK::set_and_wait(500);
	}
	return 0;
}
