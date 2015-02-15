#ifndef USB_H
#define USB_H

#include <stm32f10x.h>
#include <utils.h>
#include <usb/definitions.h>
#include <usb/endpoint.h>

namespace USB {

typedef void(*IRQ_CALLBACK)(void);
typedef void(*HANDLER_FUNCTION)(uint8_t ep, bool out);

static void default_irq_callback(void) { }
static void invalid_ep_ctr(uint8_t ep, bool out) {
	while (1);
}

template<typename PLL,
	typename DISCONNECT_PIN,
	DESCRIPTOR_CALLBACK get_descriptor_callback,
	SETUP_CALLBACK handle_setup_callback,
	IRQ_CALLBACK irq_callback,
	typename EP1_HANDLER = EP_DEFAULT_HANDLER,
	typename EP2_HANDLER = EP_DEFAULT_HANDLER,
	typename EP3_HANDLER = EP_DEFAULT_HANDLER,
	typename EP4_HANDLER = EP_DEFAULT_HANDLER,
	typename EP5_HANDLER = EP_DEFAULT_HANDLER,
	typename EP6_HANDLER = EP_DEFAULT_HANDLER,
	typename EP7_HANDLER = EP_DEFAULT_HANDLER>
struct T {
	typedef EP0_HANDLER_T<get_descriptor_callback, handle_setup_callback> EP0_HANDLER;

	static constexpr HANDLER_FUNCTION ep_to_handler(const uint8_t ep) {
		return ep == EP1_HANDLER::number ? EP1_HANDLER::ctr :
			ep == EP2_HANDLER::number ? EP2_HANDLER::ctr :
			ep == EP3_HANDLER::number ? EP3_HANDLER::ctr :
			ep == EP4_HANDLER::number ? EP4_HANDLER::ctr :
			ep == EP5_HANDLER::number ? EP5_HANDLER::ctr :
			ep == EP6_HANDLER::number ? EP6_HANDLER::ctr :
			ep == EP7_HANDLER::number ? EP7_HANDLER::ctr : invalid_ep_ctr;
	}

	static constexpr HANDLER_FUNCTION ep_handler[8] {
		EP0_HANDLER::ctr, ep_to_handler(1), ep_to_handler(2), ep_to_handler(3),
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
		if (irq_callback != default_irq_callback) {
			irq_callback();
		}
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
	}

	static void enable(void) {
	}

	static void disable(void) {
	}
};

template<typename PLL, typename DISCONNECT_PIN,
	DESCRIPTOR_CALLBACK get_descriptor_callback,
	SETUP_CALLBACK handle_setup_callback,
	IRQ_CALLBACK irq_callback,
	typename EP1_HANDLER, typename EP2_HANDLER, typename EP3_HANDLER, typename EP4_HANDLER,
	typename EP5_HANDLER, typename EP6_HANDLER, typename EP7_HANDLER>
constexpr HANDLER_FUNCTION T<PLL, DISCONNECT_PIN, get_descriptor_callback, handle_setup_callback, irq_callback,
	  EP1_HANDLER, EP2_HANDLER, EP3_HANDLER, EP4_HANDLER, EP5_HANDLER, EP6_HANDLER, EP7_HANDLER>::ep_handler[8];

}

#endif
