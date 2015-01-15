#ifndef USB_H
#define USB_H

#include <stm32f10x.h>
#include <utils.h>

namespace USB {

enum STANDARD_REQUESTS {
	GET_STATUS = 0,
	CLEAR_FEATURE = 1,
	SET_FEATURE = 3,
	SET_ADDRESS = 5,
	GET_DESCRIPTOR = 6,
	SET_DESCRIPTOR = 7,
	GET_CONFIGURATION = 8,
	SET_CONFIGURATION = 9,
	GET_INTERFACE = 10,
	SET_INTERFACE = 11,
	SYNCH_FRAME = 12
};

enum DESCRIPTOR_TYPE {
	DEVICE_DESCRIPTOR = 1,
	CONFIG_DESCRIPTOR = 2,
	STRING_DESCRIPTOR = 3,
	INTERFACE_DESCRIPTOR = 4,
	ENDPOINT_DESCRIPTOR = 5
};

enum FEATURE_SELECTOR {
	ENDPOINT_STALL,
	DEVICE_REMOTE_WAKEUP
};

#define REQUEST_TYPE      0x60  /* Mask to get request type */
#define STANDARD_REQUEST  0x00  /* Standard request */
#define CLASS_REQUEST     0x20  /* Class request */
#define VENDOR_REQUEST    0x40  /* Vendor request */
#define RECIPIENT         0x1F  /* Mask to get recipient */

enum EP_TYPE {
	BULK = 0,
	CONTROL = 1,
	ISO = 2,
	INTERRUPT = 3
};

enum EP_STATUS {
	DISABLED = 0,
	STALL = 1,
	NAK = 2,
	VALID = 3
};

struct USB_TypeDef {
	__IO uint32_t EP0R;
	__IO uint32_t EP1R;
	__IO uint32_t EP2R;
	__IO uint32_t EP3R;
	__IO uint32_t EP4R;
	__IO uint32_t EP5R;
	__IO uint32_t EP6R;
	__IO uint32_t EP7R;
	uint32_t  RESERVED0;
	uint32_t  RESERVED1;
	uint32_t  RESERVED2;
	uint32_t  RESERVED3;
	uint32_t  RESERVED4;
	uint32_t  RESERVED5;
	uint32_t  RESERVED6;
	uint32_t  RESERVED7;
	__IO uint32_t CNTR;
	__IO uint32_t ISTR;
	__IO uint32_t FNR;
	__IO uint32_t DADDR;
	__IO uint32_t BTABLE;
};

struct USB_BTABLE_TypeDef {
	uint16_t USB_ADDR_TX;
	uint16_t RESERVED0;
	uint16_t USB_COUNT_TX;
	uint16_t RESERVED1;
	uint16_t USB_ADDR_RX;
	uint16_t RESERVED2;
	uint16_t USB_COUNT_RX;
	uint16_t RESERVED3;
};

struct SETUP_PACKET_HEADER {
	uint8_t bmRequestType;
	uint8_t bRequest;
};

struct SETUP_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint16_t wValue;
	uint16_t RESERVED1;
	uint16_t wIndex;
	uint16_t RESERVED2;
	uint16_t wLength;
};

struct CLEAR_FEATURE_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint16_t wFeature;
	uint16_t RESERVED1;
	uint16_t wIndex;
};

struct GET_CONFIGURATION_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint16_t wFeature;
	uint16_t RESERVED1;
	uint16_t wIndex;
	uint16_t RESERVED2;
	uint16_t wLength;
	uint16_t RESERVED3;
	uint8_t bConfiguration;
};

struct GET_DESCRIPTOR_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint8_t bDescriptorIndex;
	uint8_t bDescriptorType;
	uint16_t RESERVED1;
	uint16_t wLanguageID;
	uint16_t RESERVED2;
	uint16_t wLength;
};

struct GET_INTERFACE_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wInterface;
	uint16_t wLength;
	uint8_t bAlternateInterface;
};

struct GET_STATUS_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t wStatus;
};

struct SET_ADDRESS_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t RESERVED0;
	uint16_t wAddress;
};

struct SET_CONFIGURATION_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wConfiguration;
};

struct SET_DESCRIPTOR_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t bDescriptorIndex;
	uint8_t bDescriptorType;
	uint16_t wLanguageID;
	uint16_t wLength;
};

struct SET_FEATURE_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wFeatureSelector;
	uint16_t wIndex;
};

struct SET_INTERFACE_PACKET {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wAlternateSetting;
	uint16_t wInterface;
};

struct SYNC_FRAME {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wEndpoint;
	uint16_t wLength;
	uint16_t wFrameNumber;
};

#define USB_BASE (uint32_t *) 0x40005c00
USB_TypeDef USB_P __attribute__((section(".usb_peripheral")));
USB_TypeDef * const USB = (USB_TypeDef *) &USB_P;

#define USB_BTABLE_BASE (uint32_t *) 0x40006000
USB_BTABLE_TypeDef USB_BTABLE[8] __attribute__((section(".usb_pma_peripheral")));
// USB_BTABLE_TypeDef * const USB_BTABLE = (USB_BTABLE_TypeDef *) &USB_BTABLE_P[0];

struct EP_DEFAULT_HANDLER {
	static constexpr uint8_t number = 255;

	static uint16_t init(uint16_t btable_offset) {
		return btable_offset;
	}

	static void ctr(uint8_t ep, bool out) {
		uint8_t data[64];
		uint16_t *pma_data, *d;
		int16_t len;
		if (out) {
			pma_data = (uint16_t *) ((USB_BTABLE[ep].USB_ADDR_RX * 2) + 0x40006000);
			len = USB_BTABLE[ep].USB_COUNT_RX & USB_COUNT0_RX_COUNT0_RX;
			d = (uint16_t *) data;
			while (len > 0) {
				*d++ = *pma_data++;
				len -= 2;
			}
		}
	}
};

template<const uint8_t NUMBER,
	const EP_TYPE TYPE,
	const uint8_t ADDRESS,
	const uint16_t MAX_TX_COUNT,
	const uint16_t MAX_RX_COUNT,
	const EP_STATUS INIT_TX_STATUS,
	const EP_STATUS INIT_RX_STATUS>
struct ENDPOINT {
	static constexpr uint8_t number = NUMBER;
	static constexpr uint8_t address = ADDRESS;
	static constexpr uint16_t max_tx_count = MAX_TX_COUNT;
	static constexpr uint16_t max_rx_count = MAX_RX_COUNT;
	static constexpr uint16_t max_rx_count_encoded =
		MAX_RX_COUNT > 62 ?
			(0x8000 | ((MAX_RX_COUNT / 32) << 10)) :
			(MAX_RX_COUNT / 2) << 10;
	static constexpr volatile uint32_t *EPxR = ((uint32_t *) &USB->EP0R) + NUMBER;
	static constexpr USB_BTABLE_TypeDef *BTABLE = ((USB_BTABLE_TypeDef *) &USB_BTABLE) + NUMBER;

	static uint16_t init(uint16_t btable_offset) {
		*EPxR = (INIT_RX_STATUS << 12) | (TYPE << 9) | (INIT_TX_STATUS << 4) | ADDRESS;
		BTABLE->USB_ADDR_RX = btable_offset; btable_offset += MAX_RX_COUNT;
		BTABLE->USB_ADDR_TX = btable_offset; btable_offset += MAX_TX_COUNT;
		reset_rx_count();
		set_tx_count(0);
		return btable_offset;
	}

	static void clear_ctr_tx(void) {
		*EPxR &= 0x8f0f;
	}

	static void clear_ctr_rx(void) {
		*EPxR &= 0x0f8f;
	}

	static void set_tx_status(const EP_STATUS status) {
		*EPxR = ((*EPxR & 0xbfbf) | 0x8080) ^ (status << 4);
	}

	static void set_rx_status(const EP_STATUS status) {
		*EPxR = ((*EPxR & 0xbfbf) | 0x8080) ^ (status << 12);
	}

	static void set_tx_rx_status(const EP_STATUS tx_status, const EP_STATUS rx_status) {
		*EPxR = ((*EPxR & 0xbfbf) | 0x8080) ^ ((rx_status << 12) | (tx_status << 4));
	}

	static void set_tx_count(uint8_t tx_count) {
		BTABLE->USB_COUNT_TX = tx_count;
	}

	static void reset_rx_count(void) {
		BTABLE->USB_COUNT_RX = max_rx_count_encoded;
	}
};

struct EP0_HANDLER {
	typedef ENDPOINT<0, CONTROL, 0, 64, 64, STALL, VALID> EP;

	static constexpr uint8_t number = 0;
	static constexpr uint8_t device_descriptor[18] = {
//		0x12, 0x01, 0x10, 0x01, 0x02, 0x00, 0x00, 0x40, 0xBA, 0x15, 0x4E, 0xC0, 0x00, 0x00, 0x01, 0x02,
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
	static const uint8_t *pending_data;
	static uint16_t pending_length;
	static uint16_t setup_request_length;
	static uint8_t setup_set_address;

	static uint16_t init(uint16_t btable_offset) {
		return EP::init(btable_offset);
	}

	static void unhandled_event(void) {
	}

	static void send_pending(uint16_t setup_request_length) {
		uint16_t length = MIN(setup_request_length, MIN(pending_length, 64));
		uint16_t *data;

		EP::set_tx_count(length);
		pending_length -= length;
		data = (uint16_t *) ((USB_BTABLE[0].USB_ADDR_TX * 2) + 0x40006000);
		if (length % 2 == 1) length++;
		while (length > 1) {
			*data = *pending_data + (*(pending_data + 1) << 8);
			data += 2;
			pending_data += 2;
			length -= 2;
		}
		EP::reset_rx_count();
		EP::set_tx_rx_status(VALID, VALID);
	}

	static void send_descriptor(uint8_t type, uint8_t index, uint16_t length) {
		switch (type) {
		case DEVICE_DESCRIPTOR: pending_data = device_descriptor; pending_length = sizeof(device_descriptor); break;
		case CONFIG_DESCRIPTOR: pending_data = config_descriptor; pending_length = sizeof(config_descriptor); break;
		case STRING_DESCRIPTOR:
			switch (index) {
				case 1: pending_data = manufacturer_string_descriptor;
					pending_length = sizeof(manufacturer_string_descriptor); break;
				default: pending_data = product_string_descriptor;
					pending_length = sizeof(product_string_descriptor); break;
			}
			break;
		default:
			pending_length = 0;
			unhandled_event();
			break;
		}
		if (pending_length) {
			send_pending(length);
		}
	}

	static void send_zero_length_data(void) {
		EP::set_tx_count(0);
		EP::set_tx_rx_status(VALID, VALID);
	}

	static void set_address(void) {
		SET_ADDRESS_PACKET *packet = (SET_ADDRESS_PACKET *) ((USB_BTABLE[0].USB_ADDR_RX * 2) + 0x40006000);
		setup_set_address = packet->wAddress;
		send_zero_length_data();
	}

	static void get_descriptor(void) {
		GET_DESCRIPTOR_PACKET *packet = (GET_DESCRIPTOR_PACKET *) ((USB_BTABLE[0].USB_ADDR_RX * 2) + 0x40006000);
		setup_request_length = packet->wLength;
		send_descriptor(packet->bDescriptorType, packet->bDescriptorIndex, packet->wLength);
	}

	static void handle_setup(void) {
		EP::clear_ctr_rx();
		EP::set_tx_rx_status(NAK, NAK);

		SETUP_PACKET_HEADER *header = (SETUP_PACKET_HEADER *) ((USB_BTABLE[0].USB_ADDR_RX * 2) + 0x40006000);
		switch (header->bRequest) {
		case SET_ADDRESS: set_address(); break;
		case GET_DESCRIPTOR: get_descriptor(); break;
		case SET_CONFIGURATION:
		case 0x20:
		case 0x22: send_zero_length_data(); break;
		case 0x21: setup_request_length = 64; pending_data = (const uint8_t *) "\x00\xc2\x01\x00\x00\x00\x08"; pending_length = 7; send_pending(64); break;
		default: unhandled_event(); send_zero_length_data(); break;
		}
	}

	static void handle_out(void) {
		EP::clear_ctr_rx();
		EP::reset_rx_count();
		EP::set_tx_rx_status(STALL, STALL);
		unhandled_event();
	}

	static void finalize_set_address(void) {
		USB->DADDR = setup_set_address | USB_DADDR_EF;
		setup_set_address = 0;
	}

	static void handle_in(void) {
		EP::clear_ctr_tx();
		if (setup_set_address != 0) {
			finalize_set_address();
		}
		if (pending_length > 0) {
			send_pending(setup_request_length);
			EP::set_tx_status(VALID);
		}
		EP::set_rx_status(VALID);
	}

	static void ctr(uint8_t ep, bool out) {
		if (out) {
			if (USB->EP0R & USB_EP0R_SETUP) {
				handle_setup();
			} else {
				handle_out();
			}
		} else {
			handle_in();
		}
	}
};

constexpr uint8_t EP0_HANDLER::device_descriptor[18];
constexpr uint8_t EP0_HANDLER::config_descriptor[63];
constexpr uint8_t EP0_HANDLER::manufacturer_string_descriptor[14];
constexpr uint8_t EP0_HANDLER::product_string_descriptor[32];
const uint8_t *EP0_HANDLER::pending_data;
uint16_t EP0_HANDLER::pending_length;
uint16_t EP0_HANDLER::setup_request_length;
uint8_t EP0_HANDLER::setup_set_address;

typedef void(*HANDLER_FUNCTION)(uint8_t ep, bool out);

template<typename PLL,
	typename DISCONNECT_PIN,
	typename EP0_HANDLER = EP_DEFAULT_HANDLER,
	typename EP1_HANDLER = EP_DEFAULT_HANDLER,
	typename EP2_HANDLER = EP_DEFAULT_HANDLER,
	typename EP3_HANDLER = EP_DEFAULT_HANDLER,
	typename EP4_HANDLER = EP_DEFAULT_HANDLER,
	typename EP5_HANDLER = EP_DEFAULT_HANDLER,
	typename EP6_HANDLER = EP_DEFAULT_HANDLER,
	typename EP7_HANDLER = EP_DEFAULT_HANDLER>
struct T {
	static void invalid_ep_ctr(uint8_t ep, bool out) {
		while (1);
	}

	static constexpr HANDLER_FUNCTION ep_to_handler(const uint8_t ep) {
		return ep == EP0_HANDLER::number ? EP0_HANDLER::ctr :
			ep == EP1_HANDLER::number ? EP1_HANDLER::ctr :
			ep == EP2_HANDLER::number ? EP2_HANDLER::ctr :
			ep == EP3_HANDLER::number ? EP3_HANDLER::ctr :
			ep == EP4_HANDLER::number ? EP4_HANDLER::ctr :
			ep == EP5_HANDLER::number ? EP5_HANDLER::ctr :
			ep == EP6_HANDLER::number ? EP6_HANDLER::ctr :
			ep == EP7_HANDLER::number ? EP7_HANDLER::ctr : invalid_ep_ctr;
	}

	static constexpr HANDLER_FUNCTION ep_handler[8] {
		ep_to_handler(0), ep_to_handler(1), ep_to_handler(2), ep_to_handler(3),
		ep_to_handler(4), ep_to_handler(5), ep_to_handler(6), ep_to_handler(7),
	};

	static void init(void) {
		DISCONNECT_PIN::set_low();
		static_assert(PLL::frequency == 72000000 || PLL::frequency == 48000000, "PLL frequency incorrect for USB");
		if (PLL::frequency == 72000000) {
			RCC->CFGR &= ~RCC_CFGR_USBPRE;
		} else if (PLL::frequency == 48000000) {
			RCC->CFGR |= RCC_CFGR_USBPRE;
		}
		RCC->APB1ENR |= RCC_APB1ENR_USBEN;
		USB->CNTR = USB_CNTR_FRES;
		USB->CNTR = 0;
		USB->ISTR = 0;
		enable_irq();
	}

	static void enable_irq(void) {
		NVIC_ClearPendingIRQ(USB_HP_CAN1_TX_IRQn);
		NVIC_ClearPendingIRQ(USBWakeUp_IRQn);
		NVIC_ClearPendingIRQ(USB_LP_CAN1_RX0_IRQn);
		NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
		NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
		NVIC_EnableIRQ(USBWakeUp_IRQn);
		USB->CNTR |= USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM;
	}

	static void disable_irq(void) {
		USB->CNTR &= ~(USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM);
		NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
		NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
		NVIC_DisableIRQ(USBWakeUp_IRQn);
	}

	static void handle_lp_irq(void) {
		if (USB->ISTR & USB_ISTR_CTR) {
			ctr();
			USB->ISTR &= ~USB_ISTR_CTR;
		}
		if (USB->ISTR & USB_ISTR_RESET) {
			reset();
			USB->ISTR &= ~USB_ISTR_RESET;
		}
		if (USB->ISTR & USB_ISTR_SUSP) {
			suspend();
			USB->ISTR &= ~USB_ISTR_SUSP;
		}
		if (USB->ISTR & USB_ISTR_WKUP) {
			wake();
			USB->ISTR &= ~USB_ISTR_WKUP;
		}
	}

	static void handle_hp_irq(void) {
		if (USB->ISTR & USB_ISTR_CTR) {
			ctr();
			USB->ISTR &= ~USB_ISTR_CTR;
		}
	}

	static void reset(void) {
		uint16_t btable_offset = 64;
		btable_offset = EP0_HANDLER::init(btable_offset);
		btable_offset = EP1_HANDLER::init(btable_offset);
		btable_offset = EP2_HANDLER::init(btable_offset);
		btable_offset = EP3_HANDLER::init(btable_offset);
		btable_offset = EP4_HANDLER::init(btable_offset);
		btable_offset = EP5_HANDLER::init(btable_offset);
		btable_offset = EP6_HANDLER::init(btable_offset);
		EP7_HANDLER::init(btable_offset);
		USB->DADDR = 0x80;
	}

	static void suspend(void) {
		USB->CNTR |= USB_CNTR_LP_MODE | USB_CNTR_FSUSP;
	}

	static void wake(void) {
		USB->CNTR &= ~USB_CNTR_LP_MODE;
		USB->CNTR = USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM;
	}

	static void ctr(void) {
		uint8_t ep = USB->ISTR & USB_ISTR_EP_ID;
		bool out = USB->ISTR & USB_ISTR_DIR;
		ep_handler[ep](ep, out);
		/*
		switch (ep) {
			case 0: EP0_HANDLER::ctr(ep, out); break;
			case 1: EP1_HANDLER::ctr(ep, out); break;
			case 2: EP2_HANDLER::ctr(ep, out); break;
			case 3: EP3_HANDLER::ctr(ep, out); break;
			case 4: EP4_HANDLER::ctr(ep, out); break;
			case 5: EP5_HANDLER::ctr(ep, out); break;
			case 6: EP6_HANDLER::ctr(ep, out); break;
			case 7: EP7_HANDLER::ctr(ep, out); break;
		}
		*/
	}

	static void enable(void) {
	}

	static void disable(void) {
	}
};

template<typename PLL, typename DISCONNECT_PIN,
	typename EP0_HANDLER, typename EP1_HANDLER, typename EP2_HANDLER, typename EP3_HANDLER,
	typename EP4_HANDLER, typename EP5_HANDLER, typename EP6_HANDLER, typename EP7_HANDLER>
constexpr HANDLER_FUNCTION T<PLL, DISCONNECT_PIN, EP0_HANDLER, EP1_HANDLER, EP2_HANDLER, EP3_HANDLER,
	EP4_HANDLER, EP5_HANDLER, EP6_HANDLER, EP7_HANDLER>::ep_handler[8];
}

#endif
