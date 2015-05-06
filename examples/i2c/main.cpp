#include <clocks.h>
#include <gpio.h>
#include <i2c.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PB, 9, OUTPUT_10MHZ, PUSH_PULL, LOW> LED2;
typedef GPIO_T<PB, 10, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SCL;
typedef GPIO_T<PB, 11, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SDA;

typedef GPIO_T<PB, 10, OUTPUT_50MHZ, PUSH_PULL, HIGH> SCL_RESET;
typedef GPIO_T<PB, 11, INPUT> SDA_RESET;

typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_PORT_T<PB, LED1, LED2, SCL, SDA> PORT_B;
typedef GPIO_PORT_T<PB, LED1, LED2, SCL_RESET, SDA_RESET> PORT_B_I2C_RESET;

typedef I2C_T<SYSCLK, 1> I2C;

uint8_t chip_id;
uint8_t ctrl_meas;
uint8_t out[4];

int main(void)
{
	HSE::init();
	PLL::init();
	SYSCLK::init();
#if 1
	PORT_B_I2C_RESET::init();
	for (int i = 0; i < 18 && SDA_RESET::is_low(); i++) {
		SCL_RESET::toggle();
		SYSTICK::set_and_wait(1);
	}
#endif
	PORT_B::init();

	LED1::set_high();
	I2C::init();
	I2C::set_slave_addr(0x5e);
	LED2::set_high();
	while (1) {
//		I2C::write_reg(0x51, 0xc0);
		chip_id = I2C::read_reg(0x51);
//		ctrl_meas = I2C::read_reg(0xf4);
//		I2C::read_reg(0xf6, out, sizeof(out));
//		SYSTICK::set_and_wait(500);
	}
	return 0;
}
