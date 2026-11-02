#include <stm32f401xc.h>
#include <stdint.h>
#define CLOCK_FREEZE 80000000 / 100
#define PIN_IN 0
#define PIN_OUT 5

void	delay(volatile int32_t t);

int main(void)
{
	char		btn_off;
	uint32_t	input;

	RCC->AHB1ENR |= 0b11;
	GPIOA->MODER &= ~(0b11 << (2 * PIN_IN));
	GPIOB->MODER &= ~(0b11 << (2 * PIN_OUT));
	GPIOB->MODER |= (0b01 << (2 * PIN_OUT));
	GPIOA->PUPDR &= ~(0b11 << (2 * PIN_IN));
	GPIOA->PUPDR |= (0b01 << (2 * PIN_IN));
	btn_off = 1;
	while (1)
	{
		input = GPIOA->IDR & (1 << PIN_IN); 
		if (input && btn_off)
		{
			GPIOB->ODR ^= (1 << PIN_OUT);
			btn_off = 0;
		}
		else if (!input)
			btn_off = 1;
		delay(CLOCK_FREEZE / 4);
	}
}

void	delay(volatile int32_t t)
{
	while (t--);
}
