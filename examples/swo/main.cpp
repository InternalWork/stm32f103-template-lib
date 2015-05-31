#include <clocks.h>
#include <gpio.h>
#include <swo.h>
#include <io.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;

typedef GPIO_T<PB, 7, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PB, 9, OUTPUT_10MHZ, PUSH_PULL, LOW> LED2;

typedef GPIO_PORT_T<PB, LED1, LED2, BUTTON> PORT_B;

typedef SWO::T TRACE;

int main(void)
{
	HSE::init();
	PLL::init();
	SYSCLK::init();
	TRACE::init();
	while (1) {
		printf<TRACE>("Ping\n");
		SYSTICK::set_and_wait(1000);
	}
}
