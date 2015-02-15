#include <clocks.h>
#include <gpio.h>

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
typedef GPIO_T<PC, 13,  OUTPUT_10MHZ, PUSH_PULL> LED1;
typedef GPIO_PORT_T<PA, D0, D1, D2, D3, D6, D7> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON, LED1> PORT_C;

int main(void)
{
	SYSCLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	while (1) {
		D0::toggle();
		LED1::toggle();
		SYSTICK::set_and_wait(1000);
	}
	return 0;
}
