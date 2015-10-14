#include <clocks.h>
#include <gpio.h>
#include <i2c.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;

#ifdef MINIBOARD
typedef GPIO_T<PC, 13, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PB, 10, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SCL;
typedef GPIO_T<PB, 11, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SDA;

typedef GPIO_T<PB, 10, OUTPUT_50MHZ, PUSH_PULL, HIGH> SCL_RESET;
typedef GPIO_T<PB, 11, INPUT> SDA_RESET;

typedef GPIO_PORT_T<PB, SCL, SDA> PORT_B;
typedef GPIO_PORT_T<PB, SCL_RESET, SDA_RESET> PORT_B_I2C_RESET;
typedef GPIO_PORT_T<PC, LED1> PORT_C;
#else
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PB, 10, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SCL;
typedef GPIO_T<PB, 11, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SDA;

typedef GPIO_T<PB, 10, OUTPUT_50MHZ, PUSH_PULL, HIGH> SCL_RESET;
typedef GPIO_T<PB, 11, INPUT> SDA_RESET;

typedef GPIO_PORT_T<PB, LED1, SCL, SDA> PORT_B;
typedef GPIO_PORT_T<PB, LED1, SCL_RESET, SDA_RESET> PORT_B_I2C_RESET;
typedef GPIO_PORT_T<PC> PORT_C;
#endif
typedef I2C::T<SYSCLK, 1> I2C_DEV;

uint8_t chip_id;
uint8_t ctrl_meas;
uint8_t out[4];

int main(void)
{
	HSE::init();
	PLL::init();
	SYSCLK::init();
	I2C_DEV::init();
#if 1
	PORT_B_I2C_RESET::init();
	for (int i = 0; i < 18 && SDA_RESET::is_low(); i++) {
		SCL_RESET::toggle();
		SYSTICK::set_and_wait(1);
	}
#endif
	PORT_B::init();
	PORT_C::init();

	LED1::set_high();
	I2C_DEV::set_slave_addr(0x5e);
	LED1::set_low();
	while (1) {
		LED1::toggle();
//		I2C_DEV::write_reg(0x51, 0xc0);
		I2C_DEV::read_reg(0x51, chip_id);
//		ctrl_meas = I2C_DEV::read_reg(0xf4);
//		I2C_DEV::read_reg(0xf6, out, sizeof(out));
//		SYSTICK::set_and_wait(500);
	}
	return 0;
}
