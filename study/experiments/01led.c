#include <stm32f401xc.h>
#include <stdint.h>
#define CLOCK_FREEZE 80000000 / 100

void	delay(volatile int32_t t);

int main(void)
{
	RCC->AHB1ENR |= 1;
	GPIOA->MODER &= (0b11 << (2 * 0));
	GPIOA->MODER |= (0b1 << (2 * 0));
	while (1)
	{
		GPIOA->ODR ^= (1 << 0);
		delay(CLOCK_FREEZE);
	}
}

void	delay(volatile int32_t t)
{
	while (t--);
}
