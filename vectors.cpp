#include <stm32f10x.h>

extern "C" {

extern void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void);
extern unsigned int __etext, __data_start__, __data_end__, __bss_start__, __bss_end__, __stack;
extern int main(void);

void GetRegistersFromStack(uint32_t *faultStackAddress, uint32_t _ipsr)
{
	/* These are volatile to try and prevent the compiler/linker optimising them
	   away as the variables never actually get used.  If the debugger won't show the
	   values of the variables, make them global my moving their declaration outside
	   of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */
	volatile uint32_t ipsr;/* Program status register. */

	r0 = faultStackAddress[ 0 ];
	r1 = faultStackAddress[ 1 ];
	r2 = faultStackAddress[ 2 ];
	r3 = faultStackAddress[ 3 ];

	r12 = faultStackAddress[ 4 ];
	lr = faultStackAddress[ 5 ];
	pc = faultStackAddress[ 6 ];
	psr = faultStackAddress[ 7 ];
	ipsr = _ipsr;
	while (1);
}

void Reset_Handler(void) __attribute__((naked));
void HardFault_Handler(void) __attribute__((naked));
void Default_Handler(void) __attribute((naked));
void NMI_Handler(void) __attribute((naked));

void HardFault_Handler(void)
{
	__asm volatile
		(
		 " tst lr, #4                                                \n"
		 " ite eq                                                    \n"
		 " mrseq r0, msp                                             \n"
		 " mrsne r0, psp                                             \n"
		 " ldr r1, [r0, #24]                                         \n"
		 " mrs r1, ipsr                                              \n"
		 " ldr r2, handler2_address_const                            \n"
		 " bx r2                                                     \n"
		 " handler2_address_const: .word GetRegistersFromStack       \n"
		);
}

void NMI_Handler(void)
{
	while (1);
}

void Default_Handler(void)
{
	__asm volatile("mrs r0, ipsr");
}

void Reset_Handler(void)
{
	unsigned int *src, *dst;

	src = &__etext;
	dst = &__data_start__;
	while (dst < &__data_end__) {
		*dst = *src;
		src++;
		dst++;
	}

	dst = &__bss_start__;
	while (dst < &__bss_end__) {
		*dst = 0;
		dst++;
	}
	main();
	while (1);
}

#if defined (STM32F10X_LD)
void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_UP_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USBWakeUp_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN1_TX_IRQHandler,
	USB_LP_CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler,
	TIM1_UP_IRQHandler,
	TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	0,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	0,
	0,
	SPI1_IRQHandler,
	0,
	USART1_IRQHandler,
	USART2_IRQHandler,
	0,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	USBWakeUp_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void *) 0xF108F85F};

#elif defined (STM32F10X_LD_VL)

void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_TIM15_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CEC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM6_DAC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_IRQHandler,
	0,
	0,
	0,
	0,
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM15_IRQHandler,
	TIM1_UP_TIM16_IRQHandler,
	TIM1_TRG_COM_TIM17_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	0,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	0,
	0,
	SPI1_IRQHandler,
	0,
	USART1_IRQHandler,
	USART2_IRQHandler,
	0,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	CEC_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	TIM6_DAC_IRQHandler,
	TIM7_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void *) 0xF108F85F};

#elif defined (STM32F10X_MD)

void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_UP_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USBWakeUp_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN1_TX_IRQHandler,
	USB_LP_CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler,
	TIM1_UP_IRQHandler,
	TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	USBWakeUp_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void (*)()) 0xF108F85F};

#elif defined (STM32F10X_MD_VL)

void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_TIM15_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CEC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM6_DAC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_IRQHandler,
	0,
	0,
	0,
	0,
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM15_IRQHandler,
	TIM1_UP_TIM16_IRQHandler,
	TIM1_TRG_COM_TIM17_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	CEC_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	TIM6_DAC_IRQHandler,
	TIM7_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void *) 0xF108F85F};

#elif defined (STM32F10X_HD)

void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_UP_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USBWakeUp_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM8_BRK_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM8_UP_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM8_TRG_COM_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM8_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FSMC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SDIO_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void UART4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void UART5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel4_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN1_TX_IRQHandler,
	USB_LP_CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler,
	TIM1_UP_IRQHandler,
	TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	USBWakeUp_IRQHandler,
	TIM8_BRK_IRQHandler,
	TIM8_UP_IRQHandler,
	TIM8_TRG_COM_IRQHandler,
	TIM8_CC_IRQHandler,
	ADC3_IRQHandler,
	FSMC_IRQHandler,
	SDIO_IRQHandler,
	TIM5_IRQHandler,
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_IRQHandler,
	TIM7_IRQHandler,
	DMA2_Channel1_IRQHandler,
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	DMA2_Channel4_5_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void *) 0xF1E0F85F};

#elif defined (STM32F10X_HD_VL)

void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_TIM15_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CEC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM12_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM13_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM14_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void UART4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void UART5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM6_DAC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel4_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_IRQHandler,
	0,
	0,
	0,
	0,
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM15_IRQHandler,
	TIM1_UP_TIM16_IRQHandler,
	TIM1_TRG_COM_TIM17_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	CEC_IRQHandler,
	TIM12_IRQHandler,
	TIM13_IRQHandler,
	TIM14_IRQHandler,
	0,
	0,
	0,
	0,
	TIM5_IRQHandler,
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_DAC_IRQHandler,
	TIM7_IRQHandler,
	DMA2_Channel1_IRQHandler,
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	DMA2_Channel4_5_IRQHandler,
	DMA2_Channel5_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void *) 0xF108F85F};

#elif defined (STM32F10X_XL)

void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USBWakeUp_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM8_BRK_TIM12_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM8_UP_TIM13_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM8_TRG_COM_TIM14_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM8_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FSMC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SDIO_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void UART4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void UART5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel4_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN1_TX_IRQHandler,
	USB_LP_CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM9_IRQHandler,
	TIM1_UP_TIM10_IRQHandler,
	TIM1_TRG_COM_TIM11_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	USBWakeUp_IRQHandler,
	TIM8_BRK_TIM12_IRQHandler,
	TIM8_UP_TIM13_IRQHandler,
	TIM8_TRG_COM_TIM14_IRQHandler,
	TIM8_CC_IRQHandler,
	ADC3_IRQHandler,
	FSMC_IRQHandler,
	SDIO_IRQHandler,
	TIM5_IRQHandler,
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_IRQHandler,
	TIM7_IRQHandler,
	DMA2_Channel1_IRQHandler,
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	DMA2_Channel4_5_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void *) 0xF1E0F85F};

#elif defined (STM32F10X_CL)

void MemManage_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias ("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TAMPER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA1_Channel7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ADC1_2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_TX_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_RX1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN1_SCE_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_BRK_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_UP_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void RTCAlarm_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void OTG_FS_WKUP_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void SPI3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void UART4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void UART5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM6_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel2_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel3_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel4_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void DMA2_Channel5_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ETH_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void ETH_WKUP_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN2_TX_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN2_RX1_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void CAN2_SCE_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));
void OTG_FS_IRQHandler(void) __attribute__((weak, alias ("Default_Handler")));

void __attribute__ ((section (".isr_vector"))) (*const irq_vectors[])(void) = {
	(void (*)()) &__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	CAN1_TX_IRQHandler,
	CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler,
	TIM1_UP_IRQHandler,
	TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTCAlarm_IRQHandler,
	OTG_FS_WKUP_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	TIM5_IRQHandler,
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_IRQHandler,
	TIM7_IRQHandler,
	DMA2_Channel1_IRQHandler,
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	DMA2_Channel4_IRQHandler,
	DMA2_Channel5_IRQHandler,
	ETH_IRQHandler,
	ETH_WKUP_IRQHandler,
	CAN2_TX_IRQHandler,
	CAN2_RX0_IRQHandler,
	CAN2_RX1_IRQHandler,
	CAN2_SCE_IRQHandler,
	OTG_FS_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(void *) 0xF1E0F85F};

#else
#error "MCU not defined"
#endif

}
