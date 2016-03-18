#include <clocks.h>
#include <gpio.h>
#include <uart.h>
#include <spi.h>
#include <pwr.h>
#include <exti.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;

#ifdef OLIMEXINO
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
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> LED1;

typedef GPIO_PORT_T<PA, D0, D1, D2, LED1, D6, TX, RX, CSN, SCK, MISO, MOSI> PORT_A;
typedef GPIO_PORT_T<PB, D4, D5> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON> PORT_C;
#else
typedef GPIO_T<PA, 4, OUTPUT_50MHZ, PUSH_PULL, HIGH> CSN;
typedef GPIO_T<PA, 5, OUTPUT_50MHZ, ALT_PUSH_PULL> SCK;
typedef GPIO_T<PA, 6, INPUT> MISO;
typedef GPIO_T<PA, 7, OUTPUT_50MHZ, ALT_PUSH_PULL> MOSI;
struct USB_DISCONNECT: public GPIO_T<PA, 8, OUTPUT_50MHZ, PUSH_PULL, LOW> {
	static void set_high(void) {
		port->BRR = bit_value;
	};

	static void set_low(void) {
		port->BSRR = bit_value;
	}

	static void set(bool value) {
		if (value) set_low();
		else set_high();
	}

};

typedef GPIO_T<PB, 7, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PB, 9, OUTPUT_10MHZ, PUSH_PULL, LOW> LED2;
typedef GPIO_T<PB, 11, OUTPUT_10MHZ, PUSH_PULL, HIGH> CSN_2;
typedef GPIO_T<PB, 12, OUTPUT_10MHZ, PUSH_PULL, LOW> D0;
typedef GPIO_T<PB, 13, OUTPUT_10MHZ, PUSH_PULL, LOW> D1;
typedef GPIO_T<PB, 14, OUTPUT_10MHZ, PUSH_PULL, LOW> D2;
typedef GPIO_T<PB, 15, OUTPUT_10MHZ, PUSH_PULL, LOW> D3;

typedef GPIO_PORT_T<PA, SCK, MISO, MOSI, CSN, USB_DISCONNECT> PORT_A;
typedef GPIO_PORT_T<PB, LED1, LED2, BUTTON, CSN_2, D0, D1, D2, D3> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;
#endif
typedef USART_T<USART_1, SYSCLK, 115200> UART;
typedef SPI_T<SPI_1, SYSCLK, true, 0> SPI;
typedef EXTI_T<BUTTON> EXT_INTERRUPT;

extern "C" {

void EXTI9_5_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<5, 9>();
	LED2::toggle();
	exit_idle();
}

void USART1_IRQHandler(void)
{
	if (UART::handle_irq()) exit_idle();
}

void SPI1_IRQHandler(void)
{
	if (SPI::handle_irq()) exit_idle();
}

void SysTick_Handler(void) {
	if (TIMEOUT::count_down()) exit_idle();
}

}

static uint8_t tx_buffer[32];
static uint8_t rx_buffer[32];

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
	UART::init();
	UART::puts("SPI test start\n");
	SPI::init();
	D0::set_high();
	for (int i = 0; i < sizeof(tx_buffer); i++) tx_buffer[i] = i % 256;
	while (1) {
		uint8_t buffer[32];
		LED1::toggle();
		D0::set_low();
//		UART::puts("Transfer...\n");
		CSN::set_low();
//		SPI::transfer(0x44);
//		SPI::transfer((uint8_t *) "\x55\xaa\x12\x34", 4);
//		SPI::transfer((uint8_t *) "\x40\x00", 2);
//		SPI::receive(buffer, 2);
		SPI::transfer(tx_buffer, sizeof(tx_buffer), rx_buffer);
		for (int i = 0; i < sizeof(tx_buffer); i++) {
			if (tx_buffer[i] != rx_buffer[i]) {
				LED2::set_high();
				while (1);
			}
		}
		CSN::set_high();
		TIMEOUT::set_and_wait(10);
	}
	return 0;
}
