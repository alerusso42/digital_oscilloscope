#include <stm32f401xc.h>
#include <stdint.h>
#define CLOCK_FREEZE 80000000 / 100

void	delay(volatile int32_t t);

int main(void)
{
	RCC->AHB1ENR |= 1;
	GPIOA->MODER &= ~(0b11 << (2 * 0));
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

/*
	//SECTION - AND
	1 & 1	|	1
	1 & 0	|	0
	0 & 1	|	0
	0 & 0	|	0
	utilizzo: togliere bit specifici
	esempio: 1111 & 0110 = 0110
	//SECTION - OR
	1 | 1	|	1
	1 | 0	|	1
	0 | 1	|	1
	0 | 0	|	0
	utilizzo: aggiungere bit specifici
	esempio: 1001 | 1100 = 1101
	//SECTION - NOT
	~1		|	0
	~0		|	1
	utilizzo: invertire TUTTA una sequenza di bit
	esempio: ~1111 = 0000
	//SECTION - XOR
	1 ^ 1	|	0
	1 ^ 0	|	1
	0 ^ 1	|	1
	0 ^ 0	|	0
	utilizzo: invertire bit specifici
	esempio: 0101 ^ 1001 = 1100
	//SECTION - shift
	1 << 1	|	10
	10>> 1	|	01
	utilizzo: selezionare bit specifici in una sequenza
	esempio: 	1010 & (0011 << 2) = 1010 & 1100 = 1000 
*/