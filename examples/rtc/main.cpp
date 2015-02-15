#include <clocks.h>
#include <gpio.h>
#include <uart.h>
#include <spi.h>
#include <pwr.h>
#include <rtc.h>

typedef SYSCLK_T<> SYSCLK;
typedef LSE_OSC_T<32768> LSECLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef PWR_T<> POWER;

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
typedef RTC_T<LSECLK, 128> RTCLK;
typedef TIMEOUT_T<RTCLK> TIMEOUT;

extern "C" {

void USART1_IRQHandler(void)
{
	if (UART::handle_irq()) exit_idle();
}

void RTC_IRQHandler(void)
{
	RTCLK::handle_irq();
	if (TIMEOUT::count_down()) exit_idle();
}

}

int main(void)
{
	POWER::init();
	SYSCLK::init();
	LSECLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	RTCLK::init();
	UART::init();
	UART::puts("RTC test start\n");
	while (1) {
		TIMEOUT::set_and_wait(500);
		LED_YELLOW::toggle();
	}
	return 0;
}
