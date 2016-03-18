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
#include <usb/usb.h>
#include <usb/cp2102.h>
#include <drivers/cc110x.h>
#include <drivers/ringbuffer.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef PWR_T<> POWER;

#ifdef OLIMEXINO
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL, LOW> NRF24_CE;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_RED;
typedef GPIO_T<PA, 2, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> CC1101_IRQ;
typedef GPIO_T<PA, 3, OUTPUT_50MHZ, PUSH_PULL, HIGH> CC1101_CSN;
typedef GPIO_T<PA, 4, OUTPUT_50MHZ, PUSH_PULL, HIGH> NRF24_CSN;
typedef GPIO_T<PA, 5, OUTPUT_50MHZ, ALT_PUSH_PULL> SCK;
typedef GPIO_T<PA, 6, INPUT> MISO;
typedef GPIO_T<PA, 7, OUTPUT_50MHZ, ALT_PUSH_PULL> MOSI;

typedef GPIO_T<PC, 0, INPUT, FLOATING, HIGH, INTERRUPT_ENABLED, EDGE_FALLING> NRF24_IRQ;
typedef GPIO_T<PC, 13, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_GREEN;
typedef GPIO_T<PC, 9, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PC, 11, OUTPUT_50MHZ, PUSH_PULL, LOW> USB_DISCONNECT;

typedef GPIO_PORT_T<PA, SCK, MISO, MOSI, NRF24_CSN, NRF24_CE, CC1101_CSN, CC1101_IRQ, LED_RED> PORT_A;
typedef GPIO_PORT_T<PB> PORT_B;
typedef GPIO_PORT_T<PC, NRF24_IRQ, BUTTON, USB_DISCONNECT, LED_GREEN> PORT_C;
#else
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL, LOW> NRF24_CE;
typedef GPIO_T<PA, 1, INPUT, FLOATING, HIGH, INTERRUPT_ENABLED, EDGE_FALLING> NRF24_IRQ;
typedef GPIO_T<PA, 2, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> CC1101_IRQ;
typedef GPIO_T<PA, 3, OUTPUT_50MHZ, PUSH_PULL, HIGH> CC1101_CSN;
typedef GPIO_T<PA, 4, OUTPUT_50MHZ, PUSH_PULL, HIGH> NRF24_CSN;
typedef GPIO_T<PA, 5, OUTPUT_50MHZ, ALT_PUSH_PULL> SCK;
typedef GPIO_T<PA, 6, INPUT> MISO;
typedef GPIO_T<PA, 7, OUTPUT_50MHZ, ALT_PUSH_PULL> MOSI;
typedef GPIO_T<PA, 8, OUTPUT_10MHZ, PUSH_PULL, HIGH> USB_CONNECT;

typedef GPIO_T<PB, 7, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;
typedef GPIO_T<PB, 8, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_RED;
typedef GPIO_T<PB, 9, OUTPUT_10MHZ, PUSH_PULL, LOW> LED_GREEN;
typedef GPIO_T<PB, 13, INPUT> MISO_IN;

typedef GPIO_PORT_T<PA, SCK, MISO, MOSI, NRF24_CSN, NRF24_CE, CC1101_CSN, NRF24_IRQ, CC1101_IRQ, USB_CONNECT> PORT_A;
typedef GPIO_PORT_T<PB, LED_RED, LED_GREEN, BUTTON, MISO_IN> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;
#endif

typedef EXTI_T<CC1101_IRQ, BUTTON> EXT_INTERRUPT;

typedef RINGBUFFER::T<uint8_t, 128> TX_BUFFER;
typedef RINGBUFFER::T<uint8_t, 256> RX_BUFFER;
typedef USB::CP2102_UART_T<PLL, TX_BUFFER, RX_BUFFER> UART;

typedef SPI_T<SPI_1, SYSCLK, true, 0, 4000000> SPI;
typedef CC110X_T<SPI, CC1101_CSN, MISO_IN, CC1101_IRQ, SYSTICK, 0> CC1101;

extern "C" {

void SPI1_IRQHandler(void)
{
	if (SPI::handle_irq()) exit_idle();
}

void EXTI0_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<0>();
	exit_idle();
}

void EXTI1_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<1>();
	exit_idle();
}

void EXTI2_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<2>();
	exit_idle();
}

void EXTI3_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<3>();
	exit_idle();
}

void EXTI4_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<4>();
	exit_idle();
}

void EXTI9_5_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<5, 9>();
	exit_idle();
}

void EXTI15_10_IRQHandler(void)
{
	EXT_INTERRUPT::handle_irq<10, 15>();
	exit_idle();
}

}

