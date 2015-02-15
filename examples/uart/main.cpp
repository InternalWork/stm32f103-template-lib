#include <clocks.h>
#include <gpio.h>
#include <uart.h>
#include <io.h>
#include <pwr.h>

typedef SYSCLK_T<> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef PWR_T<> POWER;

typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, OUTPUT_10MHZ, PUSH_PULL> D5;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL> D6;

typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> LED_YELLOW;
typedef GPIO_T<PA, 4, OUTPUT_10MHZ, PUSH_PULL> LED_GREEN;
typedef GPIO_T<PA, 9, OUTPUT_10MHZ, ALT_PUSH_PULL> TX;
typedef GPIO_T<PA, 10, INPUT> RX;
typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;

typedef GPIO_PORT_T<PA, D0, D1, D2, LED_YELLOW, LED_GREEN, D6, TX, RX> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON> PORT_C;
typedef USART_T<USART_1, SYSCLK, 115200> UART;

extern "C" {

void USART1_IRQHandler(void)
{
	if (UART::handle_irq()) exit_idle();
}

}

int main(void)
{
	char *s;
	SYSCLK::init();
	POWER::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	LED_YELLOW::set_low();
	UART::init();
	UART::puts("Hello, world!\n");
	printf<UART>("[%08x][%d]\n", 0x12345678, 9);
	s = itoa_ext(0xabcd1234, 16, 0, 1, 0);
	UART::puts(s);
	UART::putc('\n');
	POWER::sleep();
	return 0;
}
