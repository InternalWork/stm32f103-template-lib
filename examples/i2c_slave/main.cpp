#include <clocks.h>
#include <gpio.h>
#include <i2c.h>

typedef HSE_OSC_T<> HSE;
//typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<HSE> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef GPIO_T<PA, 8, OUTPUT_50MHZ, ALT_PUSH_PULL, LOW> MCO;
typedef GPIO_T<PB, 10, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SCL;
typedef GPIO_T<PB, 11, OUTPUT_50MHZ, ALT_OPENDRAIN, HIGH> SDA;
typedef GPIO_T<PB, 10, OUTPUT_50MHZ, PUSH_PULL, HIGH> SCL_RESET;
typedef GPIO_T<PB, 11, INPUT> SDA_RESET;

#ifdef MINIBOARD
typedef GPIO_T<PC, 13, OUTPUT_10MHZ, PUSH_PULL, HIGH> LED1;
typedef GPIO_PORT_T<PA, MCO> PORT_A;
typedef GPIO_PORT_T<PB, SCL, SDA> PORT_B;
typedef GPIO_PORT_T<PB, SCL_RESET, SDA_RESET> PORT_B_I2C_RESET;
typedef GPIO_PORT_T<PC, LED1> PORT_C;
#else
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, HIGH> LED1;
typedef GPIO_PORT_T<PA, MCO> PORT_A;
typedef GPIO_PORT_T<PB, LED1, SCL, SDA> PORT_B;
typedef GPIO_PORT_T<PB, LED1, SCL_RESET, SDA_RESET> PORT_B_I2C_RESET;
typedef GPIO_PORT_T<PC> PORT_C;
#endif
typedef I2C::T<SYSCLK, 2, false, 100000, false, 0x5e> I2C_DEV;

int main(void)
{
	volatile uint32_t data;
	HSE::init();
	//PLL::init();
	SYSCLK::init();
	I2C_DEV::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;

	while (1) {
		uint32_t sr1, sr2;

		sr1 = I2C_DEV::I2C->SR1;
		sr2 = I2C_DEV::I2C->SR2;
//		if (I2C_DEV::I2C->SR2 & I2C_SR2_MSL) {
			if (sr1 & I2C_SR1_ADDR) {
//				I2C_DEV::I2C->SR1;
//				I2C_DEV::I2C->SR2;
			}
			if (sr1 & I2C_SR1_TXE) {
				I2C_DEV::I2C->DR = 0x00;
			}
			if (I2C_DEV::I2C->SR1 & I2C_SR1_RXNE) {
				data = I2C_DEV::I2C->DR;
			}
			if (I2C_DEV::I2C->SR1 & I2C_SR1_STOPF) {
				I2C_DEV::I2C->CR1 |= I2C_CR1_PE;
			}
			//data = I2C_DEV::I2C->CR1;
			//I2C_DEV::I2C->CR1 = data;
//		} else {
//			while (1);
//		}
	}
	return 0;
}
