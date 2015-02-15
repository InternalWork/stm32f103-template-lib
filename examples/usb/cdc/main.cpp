#include <clocks.h>
#include <gpio.h>
#include <usb.h>
#include <ringbuffer.h>

typedef HSE_OSC_T<> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef GPIO_T<PA, 3, OUTPUT_10MHZ, PUSH_PULL> D0;
typedef GPIO_T<PA, 2, OUTPUT_10MHZ, PUSH_PULL> D1;
typedef GPIO_T<PA, 0, OUTPUT_10MHZ, PUSH_PULL> D2;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL> D3;
typedef GPIO_T<PA, 1, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_T<PA, 5, OUTPUT_10MHZ, PUSH_PULL, HIGH> LED2;
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
typedef RINGBUFFER::T<uint8_t, 256> BUFFER;

struct USB_CDC {
	static constexpr uint8_t device_descriptor[18] = {
		0x12, 0x01, 0x10, 0x01, 0x02, 0x00, 0x00, 0x40, 0xD8, 0x04, 0x0A, 0x00, 0x00, 0x01, 0x01, 0x02,
		0x00, 0x01
	};
	static constexpr uint8_t config_descriptor[63] = {
		0x09, 0x02, 0x3E, 0x00, 0x02, 0x01, 0x00, 0xC0, 0x80,
		0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x00,
		0x05, 0x24, 0x01, 0x00, 0x01,
		0x04, 0x24, 0x02, 0x06,
		0x05, 0x24, 0x06, 0x00, 0x01,
		0x07, 0x05, 0x82, 0x03, 0x40, 0x00, 0xFF,
		0x09, 0x04, 0x01, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00,
		0x07, 0x05, 0x03, 0x02, 0x40, 0x00, 0x00,
		0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00, 0x00,
	};

	static constexpr uint8_t manufacturer_string_descriptor[14] = {
		0x0E, 0x03, 0x4F, 0x00, 0x6C, 0x00, 0x69, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x78, 0x00
	};
	static constexpr uint8_t product_string_descriptor[32] = {
		0x20, 0x03, 0x4F, 0x00, 0x6C, 0x00, 0x69, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x78, 0x00, 0x69, 0x00,
		0x6E, 0x00, 0x6F, 0x00, 0x2D, 0x00, 0x53, 0x00, 0x54, 0x00, 0x4D, 0x00, 0x33, 0x00, 0x32, 0x00
	};

	static bool get_descriptor(uint8_t type, uint8_t index, const uint8_t **descriptor, uint16_t *length)
	{
		bool r = true;
		switch (type) {
		case USB::DEVICE_DESCRIPTOR: *descriptor = device_descriptor; *length = sizeof(device_descriptor); break;
		case USB::CONFIG_DESCRIPTOR: *descriptor = config_descriptor; *length = sizeof(config_descriptor); break;
		case USB::STRING_DESCRIPTOR:
			switch (index) {
			case 1: *descriptor = manufacturer_string_descriptor; *length = sizeof(manufacturer_string_descriptor); break;
			default: *descriptor = product_string_descriptor; *length = sizeof(product_string_descriptor); break;
			}
			break;
		default: r = false;
		}
		return r;
	}

	static bool handle_request(USB::SETUP_PACKET *packet, const uint8_t **response_data, uint16_t *length) {
		bool r = true;
		switch (packet->bRequest) {
		case 0x20:
		case 0x22: *length = 0; break;
		case 0x21: *response_data = (const uint8_t *) "\x00\xc2\x01\x00\x00\x00\x08"; *length = 7; break;
		default: r = false;
		}
		return r;
	}

	template<const uint8_t NUMBER,
		const USB::EP_TYPE TYPE,
		const uint8_t ADDRESS,
		const uint8_t TX_SIZE,
		const uint8_t RX_SIZE,
		const USB::EP_STATUS INIT_TX_STATUS,
		const USB::EP_STATUS INIT_RX_STATUS,
		typename INDICATOR>
	struct GENERIC_HANDLER {
		typedef USB::ENDPOINT<NUMBER, TYPE, ADDRESS, TX_SIZE, RX_SIZE, INIT_TX_STATUS, INIT_RX_STATUS> EP;

		static constexpr uint8_t number = NUMBER;

		static uint16_t init(uint16_t btable_offset) {
			return EP::init(btable_offset);
		}

		static void ctr(uint8_t ep, bool out) {
			if (out) {
				INDICATOR::toggle();
				EP::clear_ctr_rx();
				EP::reset_rx_count();
				EP::set_tx_rx_status(USB::NAK, USB::VALID);
				EP::template copy_out<BUFFER>();
			} else {
				EP::clear_ctr_tx();
			}
		}
	};

	typedef GENERIC_HANDLER<1, USB::BULK, 1, 64, 0, USB::NAK, USB::DISABLED, LED2> IN_HANDLER;
	typedef GENERIC_HANDLER<2, USB::INTERRUPT, 2, 16, 0, USB::NAK, USB::DISABLED, LED2> CONTROL_HANDLER;
	typedef GENERIC_HANDLER<3, USB::BULK, 3, 0, 64, USB::DISABLED, USB::VALID, LED2> OUT_HANDLER;

};

constexpr uint8_t USB_CDC::device_descriptor[18];
constexpr uint8_t USB_CDC::config_descriptor[63];
constexpr uint8_t USB_CDC::manufacturer_string_descriptor[14];
constexpr uint8_t USB_CDC::product_string_descriptor[32];

typedef USB::T<PLL, USB_DISCONNECT, USB_CDC::get_descriptor, USB_CDC::handle_request, USB_CDC::IN_HANDLER, USB_CDC::CONTROL_HANDLER, USB_CDC::OUT_HANDLER> USB_DRIVER;

extern "C" {

void USB_HP_CAN1_TX_IRQHandler(void) {
	USB_DRIVER::handle_hp_irq();
}

void USB_LP_CAN1_RX0_IRQHandler(void) {
	USB_DRIVER::handle_lp_irq();
}

}

int main(void)
{
	HSE::init();
	PLL::init();
	SYSCLK::init();
	PORT_A::init();
	PORT_B::init();
	PORT_C::init();
	USB_DRIVER::init();
	while(1) {
		LED1::toggle();
		SYSTICK::set_and_wait(1000);
	}
	return 0;
}
