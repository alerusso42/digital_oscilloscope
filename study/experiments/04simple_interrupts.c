#include <stm32f401xc.h>
#include <stdint.h>
#define CLOCK_FREEZE 80000000 / 100
#define PIN_IN 0
#define PIN_OUT 5

void	delay(volatile int32_t t);

int main(void)
{
	RCC->AHB1ENR |= 0b11;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0] &= ~(0x0F);
	// SYSCFG->EXTICR[0] |= 0x0F;
	EXTI->RTSR &= ~1;
	EXTI->FTSR |= 1;
	EXTI->IMR |= (1 << PIN_IN);
	GPIOA->MODER &= ~(0b11 << (2 * PIN_IN));
	GPIOB->MODER &= ~(0b11 << (2 * PIN_OUT));
	GPIOB->MODER |= (0b01 << (2 * PIN_OUT));
	GPIOA->PUPDR &= ~(0b11 << (2 * PIN_IN));
	GPIOA->PUPDR |= (0b01 << (2 * PIN_IN));
	NVIC_EnableIRQ(EXTI0_IRQn);
	while (1)
	{
		delay(CLOCK_FREEZE);
	}
}

void	delay(volatile int32_t t)
{
	while (t--);
}

void	EXTI0_IRQHandler(void)
{
	uint32_t	input;

	if ((EXTI->PR & (1 << PIN_IN)) == 0)
		return ;
	input = GPIOA->IDR & (1 << PIN_IN);
	if (input)
	{
		EXTI->PR |= (1 << PIN_IN);
		return ;
	}
	GPIOB->ODR ^= (1 << PIN_OUT);
	EXTI->PR |= (1 << PIN_IN);
}
