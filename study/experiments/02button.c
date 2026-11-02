#include <stm32f401xc.h>
#include <stdint.h>
#define CLOCK_FREEZE 80000000 / 100
#define PIN_IN 0
#define PIN_OUT 5

void	delay(volatile int32_t t);

int main(void)
{
	RCC->AHB1ENR |= 0b11;
	GPIOA->MODER &= ~(0b11 << (2 * PIN_IN));
	GPIOB->MODER &= ~(0b11 << (2 * PIN_OUT));
	GPIOB->MODER |= (0b01 << (2 * PIN_OUT));
	GPIOA->PUPDR &= ~(0b11 << (2 * PIN_IN));
	GPIOA->PUPDR |= (0b01 << (2 * PIN_IN));
	while (1)
	{
		if (GPIOA->IDR & (1 << PIN_IN))
			GPIOB->ODR &= ~(1 << PIN_OUT);
		else
			GPIOB->ODR |= (1 << PIN_OUT);
		delay(CLOCK_FREEZE / 4);
	}
}

void	delay(volatile int32_t t)
{
	while (t--);
}
