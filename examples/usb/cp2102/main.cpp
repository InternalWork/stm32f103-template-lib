#include <clocks.h>
#include <gpio.h>
#include <ringbuffer.h>
#include <usb/usb.h>
#include <usb/cp2102.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> D3;
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PB, 9, OUTPUT_10MHZ, PUSH_PULL, HIGH> LED2;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, OUTPUT_10MHZ, PUSH_PULL> D5;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL> D6;
typedef GPIO_T<PA, 9, OUTPUT_10MHZ, PUSH_PULL> D7;
typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PC, 12, OUTPUT_50MHZ, PUSH_PULL, HIGH> USB_DISCONNECT;
typedef GPIO_T<PC, 13, OUTPUT_10MHZ, PUSH_PULL, HIGH> LED3;
typedef GPIO_PORT_T<PA, LED1, LED2> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON, USB_DISCONNECT, LED3> PORT_C;

typedef RINGBUFFER::T<uint8_t, 128> TX_BUFFER;
typedef RINGBUFFER::T<uint8_t, 256> RX_BUFFER;
typedef USB::CP2102_UART_T<PLL, USB_DISCONNECT, TX_BUFFER, RX_BUFFER> UART;

extern "C" {

void USB_HP_CAN1_TX_IRQHandler(void) {
	UART::handle_hp_irq();
}

void USB_LP_CAN1_RX0_IRQHandler(void) {
	UART::handle_lp_irq();
}

}

int main(void)
{
	uint32_t count = 0;
	char buffer[128];

	HSE::init();
	PLL::init();
	SYSCLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	UART::init();
	UART::puts("Echo test\n");
	while(1) {
		LED2::toggle();
		printf<UART>("[%d] Enter a string:\n", count++);
		while (!UART::gets(buffer, sizeof(buffer))) {
			enter_idle();
		}
		UART::puts(buffer);
		UART::putc('\n');
	}
	return 0;
}
