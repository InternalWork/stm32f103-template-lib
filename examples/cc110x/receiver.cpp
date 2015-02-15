// #define USE_SOFT_SPI
// #define SENDING
#define RECEIVING

#include <spi.h>
#include <uart.h>
#include <gpio.h>
#include <exti.h>
#include <pwr.h>
#include <rtc.h>
#include <io.h>
#include <utils.h>
#include <clocks.h>
#include <tasks.h>
#include <drivers/nrf24.h>

extern constexpr uint8_t rx_addr[5] = {
	0x00, 0xf0, 0xf0, 0xf0, 0xf0
};

const uint8_t BROADCAST_ADDR[] = {0x00, 0xf0, 0xf0, 0xf0, 0xf0};

typedef SYSCLK_T<> SYSCLK;
typedef LSE_OSC_T<32768> LSECLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef PWR_T<> POWER;
typedef RTC_T<LSECLK, 128> RTCLK;
typedef TIMEOUT_T<RTCLK> TIMEOUT;

typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PB, 6, INPUT, FLOATING, HIGH, INTERRUPT_ENABLED, EDGE_FALLING> IRQ;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL> CE;

typedef GPIO_T<PA, 9, OUTPUT_10MHZ, ALT_PUSH_PULL, HIGH> TX;
typedef GPIO_T<PA, 10, INPUT> RX;

typedef GPIO_T<PA, 4, OUTPUT_50MHZ, PUSH_PULL, HIGH> CSN;
typedef GPIO_T<PA, 7, OUTPUT_50MHZ, ALT_PUSH_PULL> MOSI;
typedef GPIO_T<PA, 6, INPUT> MISO;
typedef GPIO_T<PA, 5, OUTPUT_50MHZ, ALT_PUSH_PULL> SCK;

typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> LED_YELLOW;

typedef GPIO_PORT_T<PA, D0, D1, D2, LED_YELLOW, CE, TX, RX, CSN, SCK, MISO, MOSI> PORT_A;
typedef GPIO_PORT_T<PB, D4, IRQ> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON> PORT_C;
typedef EXTI_T<IRQ> EXT_INTERRUPT;

typedef USART_T<USART_1, SYSCLK, 115200> UART;
typedef SPI_T<SPI_1, SYSCLK, true, 0, 500000> SPI;
typedef NRF24_T<SPI, CSN, CE, IRQ, SYSTICK> NRF24;

extern "C" {

void USART1_IRQHandler(void)
{
	if (UART::handle_irq()) exit_idle();
}

void SPI1_IRQHandler(void)
{
	if (SPI::handle_irq()) exit_idle();
}

void RTC_IRQHandler(void)
{
	RTCLK::handle_irq();
	if (TIMEOUT::count_down()) exit_idle();
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
	D2::set_high();
	EXT_INTERRUPT::handle_irq<5, 9>();
	D2::set_low();
}

void EXTI15_10_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<10, 15>();
}

}

uint8_t regs[64];
uint8_t packet[64];

int main(void)
{
	POWER::init();
	SYSCLK::init();
	LSECLK::init();
	RTCLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	UART::init();
	printf<UART>("NRF24 example start.\n");
	EXT_INTERRUPT::init();
	SPI::init();

	for (int i = 0; i < sizeof(regs); i++) regs[i] = 0;
	for (int i = 0; i < sizeof(packet); i++) packet[i] = 0;
	NRF24::init();
	NRF24::set_rx_addr(rx_addr);
	NRF24::set_channel(70);
	//NRF24::read_regs(regs);
	//hex_dump_bytes<UART>(regs, sizeof(regs));
#if (defined SENDING)
	int n = 0;
	while (1) {
		NRF24::start_tx();
		NRF24::tx_buffer(BROADCAST_ADDR, packet, sizeof(packet), false);
		LED_YELLOW::toggle();
		TIMEOUT::set_and_wait(500);
		if (n++ == 10) {
			NRF24::read_regs(regs);
			UART::puts("------------------\n");
			hex_dump<UART, uint8_t>(regs, sizeof(regs));
			n = 0;
		}
	}
#elif (defined SCANNING)
	NRF24::start_rx();
	while (1) {
		for (int i = 0; i < 127; i++) {
			NRF24::set_channel(i);
			printf<UART>("%03d: ", i);
			LED_YELLOW::toggle();
			for (int j = 0; j < 5; j++) {
				if (NRF24::rw_reg(RF24_RPD, RF24_NOP)) {
					UART::putc('*');
				}
				TIMEOUT::set_and_wait(1000);
			}
			UART::putc('\n');
		}
	}
#elif (defined RECEIVING)
	NRF24::start_rx();
	while (1) {
		uint8_t pipe, n;
		LED_YELLOW::toggle();
		UART::disable();
		TIMEOUT::set(10000);
		n = NRF24::rx_buffer<TIMEOUT>(packet, sizeof(packet), &pipe);
		TIMEOUT::disable();
		UART::enable();
		UART::puts("------------------\n");
		hex_dump<UART, uint8_t>(packet, n);
	}
#endif
}
