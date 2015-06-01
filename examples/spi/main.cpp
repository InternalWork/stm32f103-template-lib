#include <clocks.h>
#include <gpio.h>
#include <uart.h>
#include <spi.h>
#include <pwr.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;

typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, OUTPUT_10MHZ, PUSH_PULL> D5;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL> D6;

typedef GPIO_T<PA, 9, OUTPUT_10MHZ, ALT_PUSH_PULL> TX;
typedef GPIO_T<PA, 10, INPUT> RX;

typedef GPIO_T<PA, 4, OUTPUT_50MHZ, PUSH_PULL, HIGH> CSN;
typedef GPIO_T<PA, 7, OUTPUT_50MHZ, ALT_PUSH_PULL> MOSI;
typedef GPIO_T<PA, 6, INPUT> MISO;
typedef GPIO_T<PA, 5, OUTPUT_50MHZ, ALT_PUSH_PULL> SCK;

typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> LED_YELLOW;

typedef GPIO_PORT_T<PA, D0, D1, D2, LED_YELLOW, D6, TX, RX, CSN, SCK, MISO, MOSI> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON> PORT_C;
typedef USART_T<USART_1, SYSCLK, 115200> UART;
typedef SPI_T<SPI_1, SYSCLK, true, 0> SPI;

extern "C" {

	void USART1_IRQHandler(void)
	{
		if (UART::handle_irq()) exit_idle();
	}

	void SPI1_IRQHandler(void)
	{
		if (SPI::handle_irq()) exit_idle();
	}

}

int main(void)
{
	HSE::init();
	PLL::init();
	SYSCLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	UART::init();
	UART::puts("SPI test start\n");
	SPI::init();
	while (1) {
		UART::puts("Transfer...\n");
		CSN::set_low();
		SPI::transfer(0x44);
		SPI::transfer((uint8_t *) "\x55\xaa\x12\x34", 4);
		CSN::set_high();
		SYSTICK::set_and_wait(500);
	}
	return 0;
}
