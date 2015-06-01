#include <clocks.h>
#include <gpio.h>
#include <tasks.h>
#include <exti.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK, 1000> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

#ifdef OLIMEXINO
typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> D3;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, OUTPUT_10MHZ, PUSH_PULL> D5;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL> D6;
typedef GPIO_T<PA, 9, OUTPUT_10MHZ, PUSH_PULL> D7;
typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PC, 13,  OUTPUT_10MHZ, PUSH_PULL> LED1;
typedef GPIO_PORT_T<PA, D0, D1, D2, D3, D6, D7> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON, LED1> PORT_C;
typedef EXTI_T<BUTTON> EXT_INTERRUPT;
#else
typedef GPIO_T<PB, 7, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PB, 9, OUTPUT_10MHZ, PUSH_PULL, LOW> LED2;
typedef GPIO_T<PB, 12, OUTPUT_10MHZ, PUSH_PULL, LOW> D0;
typedef GPIO_T<PB, 13, OUTPUT_10MHZ, PUSH_PULL, LOW> D1;
typedef GPIO_T<PB, 14, OUTPUT_10MHZ, PUSH_PULL, LOW> D2;
typedef GPIO_T<PB, 15, OUTPUT_10MHZ, PUSH_PULL, LOW> D3;

typedef GPIO_PORT_T<PA> PORT_A;
typedef GPIO_PORT_T<PB, LED1, LED2, BUTTON, D0, D1, D2, D3> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;
typedef EXTI_T<BUTTON> EXT_INTERRUPT;
#endif

int n = 0;

extern "C" {

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}


void EXTI9_5_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<5, 9>();
	exit_idle();
}

void EXTI15_10_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<10, 15>();
}

}

int main(void)
{
	HSE::init();
	PLL::init();
	SYSCLK::init();
	SYSTICK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	EXT_INTERRUPT::init();
	while (1) {
		BUTTON::clear_irq();
		TIMEOUT::set(1000);
		if (BUTTON::template wait_for_irq<TIMEOUT>()) {
			LED1::set_high();
		} else {
			LED2::set_high();
		}
		SYSTICK::set_and_wait(100);
		LED1::set_low();
		LED2::set_low();
	}
	return 0;
}
