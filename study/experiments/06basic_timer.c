#include <stm32f401xc.h>
#define PIN_OUT		5
#ifndef F_CPU
# define F_CPU		84000000// 80MHz
#endif
#define PRESCALE 	8400
#define PERIOD		1000//milliseconds : 1 second delay
#define TICKS		(((F_CPU / PRESCALE) * PERIOD) / 1000)

// int	atomic_inc(volatile int *x, int exti_line);

int main(void)
{
	RCC->AHB1ENR |= 0b10;
	// SYSCFG->EXTICR[0] &= ~(0x0F);
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->PSC = PRESCALE - 1;
	TIM1->ARR = TICKS - 1;
	TIM1->DIER = TIM_DIER_UIE;//enable update interrupt
	TIM1->CNT = (1 << 16) - TICKS;//start val
	TIM1->CR1 |= TIM_CR1_CEN;//enable timer
	// EXTI->IMR |= (1 << PIN_IN);
	GPIOB->MODER &= ~(0b11 << (2 * PIN_OUT));
	GPIOB->MODER |= (0b01 << (2 * PIN_OUT));
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	while (1);
}

void TIM1_UP_TIM10_IRQHandler(void)
{
	if (!(TIM1->SR & TIM_SR_UIF))
		return ;
	TIM1->CNT += (1 << 16) - TICKS;
	GPIOB->ODR ^= (1 << PIN_OUT);
	TIM1->SR = ~TIM_SR_UIF;//interrupt off
}

// /**
//  * @brief interrupt safe x++ operator
//  * 
//  * @param x the shared variable to copy
//  * @param exti_line the exti line to block
//  * @return a copy of x
//  */
// int	atomic_inc(volatile int *x, int exti_line)
// {
// 	int	copy;

// 	NVIC_DisableIRQ(exti_line);
// 	*x += 1;
// 	copy = *x;
// 	NVIC_EnableIRQ(exti_line);
// 	return (copy);
// }
