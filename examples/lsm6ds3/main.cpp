#include <spi.h>
#include <uart.h>
#include <gpio.h>
#include <exti.h>
#include <pwr.h>
#include <rtc.h>
#include <io.h>
#include <exti.h>
#include <utils.h>
#include <clocks.h>
#include <tasks.h>
#include <usb/usb.h>
#include <usb/cp2102.h>
#include <drivers/ringbuffer.h>
#include <drivers/lsm6ds3.h>
#include <drivers/lis2dh.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;

typedef GPIO_T<PA, 4, OUTPUT_50MHZ, PUSH_PULL, HIGH> CSN_1;
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

typedef GPIO_PORT_T<PA, SCK, MISO, MOSI, CSN_1, USB_DISCONNECT> PORT_A;
typedef GPIO_PORT_T<PB, LED1, LED2, BUTTON, CSN_2, D0, D1, D2, D3> PORT_B;

typedef RINGBUFFER::T<uint8_t, 128> TX_BUFFER;
typedef RINGBUFFER::T<uint8_t, 256> RX_BUFFER;
typedef USB::CP2102_UART_T<PLL, USB_DISCONNECT, TX_BUFFER, RX_BUFFER> UART;
typedef SPI_T<SPI_1, SYSCLK, true, 3, 1000000> SPI;
typedef EXTI_T<BUTTON> EXT_INTERRUPT;
typedef LSM6DS3::T<SPI, CSN_1> SENSORS_1;
typedef LIS2DH::T<SPI, CSN_2> SENSORS_2;

extern "C" {

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

void SPI1_IRQHandler(void)
{
	if (SPI::handle_irq()) exit_idle();
}

void USB_HP_CAN1_TX_IRQHandler(void) {
	UART::handle_hp_irq();
}

void USB_LP_CAN1_RX0_IRQHandler(void) {
	UART::handle_lp_irq();
}

}

uint8_t regs[108];

int main(void)
{
	int16_t temp;
	int16_t accel_x, accel_y, accel_z;
	int16_t gyro_x, gyro_y, gyro_z;

	HSE::init();
	PLL::init();
	SYSCLK::init();
	PORT_A::init();
	PORT_B::init();
	LED1::set_high();
	USB_DISCONNECT::set_high();
	EXT_INTERRUPT::init();
	UART::init();
	SPI::init();
	SENSORS_1::init();
	SENSORS_2::init();
	while (1) {

		SENSORS_1::write_reg(0x15, 0x10);
		SENSORS_1::write_reg(0x16, 0x80);
		SENSORS_1::write_reg(0x10, 0x40);
		SENSORS_1::write_reg(0x11, 0x40);

		SENSORS_2::write_reg(0x20, 0x47);
		SYSTICK::set_and_wait(500);

		UART::puts("\033[2J\033[1;1H");
		for (int i = 0; i < sizeof(regs); i++) regs[i] = 0;
		SENSORS_1::read_regs(regs);
		temp = regs[0x20] + (regs[0x21] << 8);
		accel_x = regs[0x22] + (regs[0x23] << 8);
		accel_y = regs[0x24] + (regs[0x25] << 8);
		accel_z = regs[0x26] + (regs[0x27] << 8);
		gyro_x = regs[0x28] + (regs[0x29] << 8);
		gyro_y = regs[0x2a] + (regs[0x2b] << 8);
		gyro_z = regs[0x2c] + (regs[0x2d] << 8);
		printf<UART>("Temperature:  %d\n", temp);
		printf<UART>("Acceleration: %d %d %d\n", accel_x, accel_y, accel_z);
		printf<UART>("Gyroscope:    %d %d %d\n\n", gyro_x, gyro_y, gyro_z);
		hex_dump<UART, uint8_t>(regs, sizeof(regs), "Registers 1", true);

		UART::putc('\n');
		for (int i = 0; i < sizeof(regs); i++) regs[i] = 0;
		SENSORS_2::read_regs(regs);
		hex_dump<UART, uint8_t>(regs, 64, "Registers 2", true);

		SYSTICK::set_and_wait(1500);
	}
}
