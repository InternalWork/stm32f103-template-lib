#include <clocks.h>
#include <gpio.h>
#include <uart.h>
#include <io.h>
#include <pwr.h>

typedef SYSCLK_T<> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef PWR_T<> POWER;

#ifdef OLIMEXINO
typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, OUTPUT_10MHZ, PUSH_PULL> D5;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL> D6;

typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> LED1;
typedef GPIO_T<PA, 4, OUTPUT_10MHZ, PUSH_PULL> LED2;
typedef GPIO_T<PA, 9, OUTPUT_10MHZ, ALT_PUSH_PULL> TX;
typedef GPIO_T<PA, 10, INPUT> RX;
typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;

typedef GPIO_PORT_T<PA, D0, D1, D2, LED1, LED2, D6, TX, RX> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON> PORT_C;
#else
typedef GPIO_T<PA, 9, OUTPUT_10MHZ, ALT_PUSH_PULL> TX;
typedef GPIO_T<PA, 10, INPUT> RX;
typedef GPIO_T<PB, 7, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PB, 9, OUTPUT_10MHZ, PUSH_PULL, LOW> LED2;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL, LOW> D0;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL, LOW> D1;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL, LOW> D2;
typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL, LOW> D3;
typedef GPIO_T<PB, 12, OUTPUT_10MHZ, PUSH_PULL, LOW> D4;
typedef GPIO_T<PB, 13, OUTPUT_10MHZ, PUSH_PULL, LOW> D5;
typedef GPIO_T<PB, 14, OUTPUT_10MHZ, PUSH_PULL, LOW> D6;
typedef GPIO_T<PB, 15, OUTPUT_10MHZ, PUSH_PULL, LOW> D7;

typedef GPIO_PORT_T<PA, D0, D1, D2, D3, TX, RX> PORT_A;
typedef GPIO_PORT_T<PB, LED1, LED2, BUTTON, D4, D5, D6, D7> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;
#endif
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
	D2::set_high();
	UART::init();
	UART::puts("Hello, world!\n");
	printf<UART>("[%08x][%d]\n", 0x12345678, 9);
	s = itoa_ext(0xabcd1234, 16, 0, 1, 0);
	UART::puts(s);
	UART::putc('\n');
	POWER::sleep();
	return 0;
}
