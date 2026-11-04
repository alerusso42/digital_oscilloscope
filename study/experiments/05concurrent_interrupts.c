#include <stm32f401xc.h>
#include <stdint.h>
#define CLOCK_FREEZE 80000000 / 100
#define PIN_IN 0
#define PIN_OUT 5

void	delay(volatile int32_t t);
int	atomic_inc(volatile int *x, int exti_line);

volatile int	g_state;

int main(void)
{
	int	state;

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
		//VERSION 1
		state = atomic_inc(&g_state, EXTI0_IRQn);
		if (state % 2)
			GPIOB->BSRR |= (1 << PIN_OUT);
		else
			GPIOB->BSRR |= (1 << (PIN_OUT + 16));
		delay(CLOCK_FREEZE * 3);
		//VERSION 2
		// NVIC_DisableIRQ(EXTI0_IRQn);
		// // state = g_state++;
		// if (g_state++ % 2)
		// 	GPIOB->BSRR |= (1 << PIN_OUT);
		// else
		// 	GPIOB->BSRR |= (1 << (PIN_OUT + 16));
		// NVIC_EnableIRQ(EXTI0_IRQn);
		// delay(CLOCK_FREEZE * 3);
	}
}

void	delay(volatile int32_t t)
{
	while (t--);
}

void	EXTI0_IRQHandler(void)
{
	if ((EXTI->PR & (1 << PIN_IN)) == 0)
		return ;
	if (g_state++ % 2)
		GPIOB->BSRR |= (1 << PIN_OUT);
	else
		GPIOB->BSRR |= (1 << (PIN_OUT + 16));
	EXTI->PR |= (1 << PIN_IN);
}

/**
 * @brief interrupt safe x++ operator
 * 
 * @param x the shared variable to copy
 * @param exti_line the exti line to block
 * @return a copy of x
 */
int	atomic_inc(volatile int *x, int exti_line)
{
	int	copy;

	NVIC_DisableIRQ(exti_line);
	*x += 1;
	copy = *x;
	NVIC_EnableIRQ(exti_line);
	return (copy);
}
