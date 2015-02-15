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
#include <drivers/cc110x.h>
#include <drivers/ringbuffer.h>
#include <usb/usb.h>
#include <usb/cp2102.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef LSE_OSC_T<32768> LSECLK;
typedef RTC_T<LSECLK, 128> RTCLK;
typedef TIMEOUT_T<RTCLK> TIMEOUT;
typedef PWR_T<> POWER;

typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 4, OUTPUT_10MHZ, PUSH_PULL, HIGH> CC1101_CSN;
typedef GPIO_T<PB, 5, OUTPUT_10MHZ, PUSH_PULL> D4;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D10;
typedef GPIO_T<PA, 0, INPUT, FLOATING, HIGH, INTERRUPT_ENABLED, EDGE_FALLING> CC1101_IRQ;

typedef GPIO_T<PA, 7, OUTPUT_50MHZ, ALT_PUSH_PULL> MOSI;
typedef GPIO_T<PA, 6, INPUT> MISO;
typedef GPIO_T<PA, 5, OUTPUT_50MHZ, ALT_PUSH_PULL> SCK;

typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PC, 12, OUTPUT_50MHZ, PUSH_PULL, HIGH> USB_DISCONNECT;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_YELLOW;

typedef GPIO_PORT_T<PA, D0, D4, D10, CC1101_CSN, CC1101_IRQ, LED_YELLOW, SCK, MISO, MOSI> PORT_A;
typedef GPIO_PORT_T<PB, D4> PORT_B;
typedef GPIO_PORT_T<PC, BUTTON, USB_DISCONNECT> PORT_C;
typedef EXTI_T<CC1101_IRQ, BUTTON> EXT_INTERRUPT;

typedef RINGBUFFER::T<uint8_t, 128> TX_BUFFER;
typedef RINGBUFFER::T<uint8_t, 256> RX_BUFFER;
typedef USB::CP2102_UART_T<PLL, USB_DISCONNECT, TX_BUFFER, RX_BUFFER> UART;
typedef SPI_T<SPI_1, SYSCLK, true, 0, 4000000> SPI;
typedef CC110X_T<SPI, CC1101_CSN, MISO, CC1101_IRQ, SYSTICK, 0> CC1101;

extern "C" {

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
	EXT_INTERRUPT::handle_irq<5, 9>();
}

void EXTI15_10_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<10, 15>();
}

void USB_HP_CAN1_TX_IRQHandler(void) {
	UART::handle_hp_irq();
}

void USB_LP_CAN1_RX0_IRQHandler(void) {
	UART::handle_lp_irq();
}

}

uint8_t regs[64];
uint8_t packet[64];

int main(void)
{
	uint32_t count = 0;
	POWER::init();
	HSE::init();
	PLL::init();
	SYSCLK::init();
	LSECLK::init();
	RTCLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	UART::init();
	UART::puts("CC1101 example start.\n");
	EXT_INTERRUPT::init();
	LED_YELLOW::set_high();
	SPI::init();
	CC1101::init();
	while (1) {
		printf<UART>("Sending packet %d\n", count++);
		LED_YELLOW::toggle();
		CC1101::tx_buffer((uint8_t *) "\x55\xaa\x55\xaa", 4);
		TIMEOUT::set_and_wait(1000);
	}
}
