#ifndef USB_ENDPOINT_H
#define USB_ENDPOINT_H

namespace USB {

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

	static bool ctr_tx(void) {
		return *EPxR & 0x0080;
	}

	static bool ctr_rx(void) {
		return *EPxR & 0x8000;
	}

	template<typename BUFFER>
	static uint32_t read(void) {
		uint32_t i;
		uint32_t count = MIN(BTABLE->USB_COUNT_RX & 0x03ff, BUFFER::free_space());
		uint8_t *data = (uint8_t *) ((BTABLE->USB_ADDR_RX * 2) + 0x40006000);

		for (i = 0; i < count; i++) {
			BUFFER::write(*data);
			data++;
			if (i & 1) {
				data += 2;
			}
		}
		return i;
	}

	template<typename BUFFER>
	static uint32_t write(void) {
		uint32_t i;
		uint32_t count = MIN(MAX_TX_COUNT, BUFFER::used_space());
		uint16_t *data = (uint16_t *) ((BTABLE->USB_ADDR_TX * 2) + 0x40006000);

		for (i = 0; i < count; i += 2) {
			*data = BUFFER::read() + ((i + 1) < count ? BUFFER::read() << 8 : 0);
			data += 2;
		}
		return i;
	}
};

typedef bool(*DESCRIPTOR_CALLBACK)(uint8_t type, uint8_t index, const uint8_t **descriptor, uint16_t *length);
typedef bool(*SETUP_CALLBACK)(SETUP_PACKET *packet, const uint8_t **response_data, uint16_t *length);

template<DESCRIPTOR_CALLBACK get_descriptor_callback,
	SETUP_CALLBACK handle_setup_callback>
struct EP0_HANDLER_T {
	typedef ENDPOINT<0, CONTROL, 0, 64, 64, STALL, VALID> EP;

	static constexpr uint8_t number = 0;
	static const uint8_t *pending_data;
	static uint16_t pending_length;
	static uint16_t setup_request_length;
	static uint8_t setup_set_address;
	static uint8_t active_configuration;

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
		if (get_descriptor_callback(type, index, &pending_data, &pending_length)) {
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

		SETUP_PACKET *packet = (SETUP_PACKET *) ((USB_BTABLE[0].USB_ADDR_RX * 2) + 0x40006000);
		switch (packet->bRequest) {
		case SET_ADDRESS: set_address(); break;
		case GET_DESCRIPTOR: get_descriptor(); break;
		case SET_CONFIGURATION:
			active_configuration = packet->wValue;
			send_zero_length_data();
			break;
		default:
			if (packet->bmRequestType & REQUEST_TYPE == STANDARD_REQUEST) {
				send_zero_length_data();
			} else {
				if (handle_setup_callback(packet, &pending_data, &pending_length)) {
					send_pending(packet->wLength);
				} else {
					send_zero_length_data();
				}
			}
			break;
		}
	}

	static void handle_out(void) {
		EP::clear_ctr_rx();
		EP::reset_rx_count();
		EP::set_tx_rx_status(VALID, VALID);
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
		} else {
			EP::set_tx_count(0);
			EP::reset_rx_count();
		}
		EP::set_tx_rx_status(VALID, VALID);
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

template<DESCRIPTOR_CALLBACK GET_DESCRIPTOR, SETUP_CALLBACK HANDLE_SETUP_REQUEST>
const uint8_t *EP0_HANDLER_T<GET_DESCRIPTOR, HANDLE_SETUP_REQUEST>::pending_data;

template<DESCRIPTOR_CALLBACK GET_DESCRIPTOR, SETUP_CALLBACK HANDLE_SETUP_REQUEST>
uint16_t EP0_HANDLER_T<GET_DESCRIPTOR, HANDLE_SETUP_REQUEST>::pending_length;

template<DESCRIPTOR_CALLBACK GET_DESCRIPTOR, SETUP_CALLBACK HANDLE_SETUP_REQUEST>
uint16_t EP0_HANDLER_T<GET_DESCRIPTOR, HANDLE_SETUP_REQUEST>::setup_request_length;

template<DESCRIPTOR_CALLBACK GET_DESCRIPTOR, SETUP_CALLBACK HANDLE_SETUP_REQUEST>
uint8_t EP0_HANDLER_T<GET_DESCRIPTOR, HANDLE_SETUP_REQUEST>::setup_set_address;

template<DESCRIPTOR_CALLBACK GET_DESCRIPTOR, SETUP_CALLBACK HANDLE_SETUP_REQUEST>
uint8_t EP0_HANDLER_T<GET_DESCRIPTOR, HANDLE_SETUP_REQUEST>::active_configuration;

template<const uint8_t NUMBER,
	const EP_TYPE TYPE,
	const uint8_t ADDRESS,
	const uint8_t TX_SIZE,
	const uint8_t RX_SIZE,
	const EP_STATUS INIT_TX_STATUS,
	const EP_STATUS INIT_RX_STATUS,
	typename TX_BUFFER,
	typename RX_BUFFER>
struct BUFFER_ENDPOINT {
	typedef ENDPOINT<NUMBER, TYPE, ADDRESS, TX_SIZE, RX_SIZE, INIT_TX_STATUS, INIT_RX_STATUS> EP;

	volatile static bool transmitting;

	static constexpr uint8_t number = NUMBER;

	static uint16_t init(uint16_t btable_offset) {
		transmitting = false;
		return EP::init(btable_offset);
	}

	static void ctr(uint8_t ep, bool out) {
		if (RX_SIZE > 0 && out) {
			EP::clear_ctr_rx();
			EP::set_rx_status(NAK);
			EP::template read<RX_BUFFER>();
			EP::reset_rx_count();
			EP::set_tx_rx_status(NAK, VALID);
		} else if (TX_SIZE > 0) {
			EP::clear_ctr_tx();
			if (!TX_BUFFER::is_empty()) {
				EP::set_tx_count(EP::template write<TX_BUFFER>());
				EP::set_tx_rx_status(VALID, VALID);
			} else {
				transmitting = false;
				EP::set_tx_rx_status(NAK, VALID);
			}
		}
	}

	static void tx_start(void) {
		while (transmitting) {
			enter_idle();
		}
		transmitting = true;
		EP::set_tx_count(EP::template write<TX_BUFFER>());
		EP::set_tx_status(USB::VALID);
	}
};

template<const uint8_t NUMBER, const EP_TYPE TYPE, const uint8_t ADDRESS, const uint8_t TX_SIZE, const uint8_t RX_SIZE,
	const EP_STATUS INIT_TX_STATUS, const EP_STATUS INIT_RX_STATUS, typename TX_BUFFER, typename RX_BUFFER>
volatile bool BUFFER_ENDPOINT<NUMBER, TYPE, ADDRESS, TX_SIZE, RX_SIZE, INIT_TX_STATUS, INIT_RX_STATUS, TX_BUFFER, RX_BUFFER>::transmitting;

}

#endif
