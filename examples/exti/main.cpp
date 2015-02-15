#include <gpio.h>
#include <clocks.h>
#include <exti.h>
#include <pwr.h>
#include <utils.h>

typedef SYSCLK_T<> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef PWR_T<> POWER;
typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> LED_YELLOW;
typedef GPIO_T<PA, 4, OUTPUT_10MHZ, PUSH_PULL> LED_GREEN;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, OUTPUT_10MHZ, PUSH_PULL> D5;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL> D6;
typedef GPIO_T<PA, 9, OUTPUT_10MHZ, PUSH_PULL> D7;
typedef GPIO_T<PC, 9, INPUT, FLOATING, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_PORT_T<PA, D0, D1, D2, LED_YELLOW, LED_GREEN, D6, D7> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON> PORT_C;
typedef EXTI_T<BUTTON> EXT_INTERRUPT;

extern "C" {

void HardFault_Handler(void)
{
	PORT_A::init();
	while (1) {
		LED_GREEN::toggle();
		for (int i = 0; i < 100000; i++);
	}
}

void EXTI0_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0>();
}

void EXTI1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<1>();
}

void EXTI2_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<2>();
}

void EXTI3_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<3>();
}

void EXTI4_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<4>();
}

void EXTI9_5_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<5, 9>();
}

void EXTI15_10_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<10, 15>();
}

}

int main(void)
{
	POWER::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	EXT_INTERRUPT::init();

	while (1) {
		POWER::sleep();
	}
}
