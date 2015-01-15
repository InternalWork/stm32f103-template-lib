#ifndef __ADC_H
#define __ADC_H

enum {
	ADC_IN0 = 0,
	ADC_IN1,
	ADC_IN2,
	ADC_IN3,
	ADC_IN4,
	ADC_IN5,
	ADC_IN6,
	ADC_IN7,
	ADC_IN8,
	ADC_IN9,
	ADC_IN10,
	ADC_IN11,
	ADC_IN12,
	ADC_IN13,
	ADC_IN14,
	ADC_IN15,
	ADC_IN16,
	ADC_IN17
};

struct ADC_T {
	static void init(void) {
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		ADC1->CR |= ADC_CR_ADEN;
	}

	static void disable(void) {
		ADC1->CR |= ADC_CR_ADDIS;
		while (ADC1->CR & ADC_CR_ADDIS);
		RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;
	}

	template <typename SOURCE>
	static uint32_t sample(void) {
		ADC1->CHSELR = (1 << SOURCE::adc_channel());
		ADC1->SMPR = ADC_SMPR1_SMPR_2;
		ADC1->CR |= ADC_CR_ADSTART;
		while (!end_of_conversion());
		return ADC1->DR;
	}

	static bool end_of_conversion(void) {
		return (ADC1->ISR & ADC_ISR_EOC) != 0;
	}
};

#endif
