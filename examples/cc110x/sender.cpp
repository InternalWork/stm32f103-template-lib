#include "common.h"

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
	POWER::init();
	//HSE::init();
	//PLL::init();
	SYSCLK::init();
	PORT_A::init();
	PORT_B::init();
	LED_GREEN::set_high();
	//UART::init();
	//UART::puts("CC1101 example start.\n");
	EXT_INTERRUPT::init();
	SPI::init();
	CC1101::reset();
	CC1101::init(cc110x_default_init_values, ARRAY_COUNT(cc110x_default_init_values));
	LED_RED::set_high();
	while (1) {
		//printf<UART>("Sending packet %d\n", count++);
		LED_RED::toggle();
		CC1101::tx_buffer((uint8_t *) "\x55\xaa\x55\xaa", 4);
		SYSTICK::set_and_wait(1000);
	}
}
