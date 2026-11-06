#include <stm32f401xc.h>
#include <stdbool.h>
#define PIN_R		10
#define PIN_G		3
#define PIN_B		7
#ifndef F_CPU
# define F_CPU		84000000// 80MHz
#endif
#define PRESCALE 	8400
#define PERIOD		200//milliseconds : 1 second delay
#define TICKS		(((F_CPU / PRESCALE) * PERIOD) / 1000)

enum e_time
{
	FLAG_R = 1,
	FLAG_G = 2,
	FLAG_B = 3,
	PORT_GPIOA = 0,
	PORT_GPIOB = 1,
	MASK_COMPARE = TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF
};

//RGB LED =>	R(A10), G(A3), B(B7)

void	checkPin(volatile uint8_t *flag, int pin, int gpio);

volatile uint8_t	tick = 0;
volatile uint8_t	flagR = 0;
volatile uint8_t	flagG = 0;
volatile uint8_t	flagB = 0;

int main(void)
{
	RCC->AHB1ENR |= 0b11;
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->PSC = PRESCALE - 1;
	TIM1->CNT = 0;//start val
	TIM1->ARR = TICKS - 1;
	TIM1->DIER = TIM_DIER_UIE;//enable update interrupt
	GPIOA->MODER &= ~(0b11 << (2 * PIN_R));
	GPIOA->MODER &= ~(0b11 << (2 * PIN_G));
	GPIOB->MODER &= ~(0b11 << (2 * PIN_B));
	GPIOA->MODER |= (0b01 << (2 * PIN_R));
	GPIOA->MODER |= (0b01 << (2 * PIN_G));
	GPIOB->MODER |= (0b01 << (2 * PIN_B));
	GPIOA->ODR &= ~(1 << PIN_R);
	GPIOA->ODR &= ~(1 << PIN_G);
	GPIOB->ODR &= ~(1 << PIN_B);
	TIM1->SR = ~MASK_COMPARE;
	TIM1->EGR  = TIM_EGR_UG;//IMPORTANT: force timer data update
	TIM1->CR1 |= TIM_CR1_CEN;//enable timer
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	while (1)
	{
		if (!tick)
			continue ;
		if (++flagR == FLAG_R)
		{
			GPIOA->ODR ^= (1 << PIN_R);
			flagR = 0;
		}
		if (++flagG == FLAG_G)
		{
			GPIOA->ODR ^= (1 << PIN_G);
			flagG = 0;
		}
		if (++flagB == FLAG_B)
		{
			GPIOB->ODR ^= (1 << PIN_B);
			flagB = 0;
		}
		tick = 0;
	}
}

void TIM1_UP_TIM10_IRQHandler(void)
{
	if (!(TIM1->SR & TIM_SR_UIF))
		return ;
	tick = 1;
	TIM1->SR &= ~TIM_SR_UIF;
}

/*
	R(1 on, 1 stop)	G(2 on, 2 pause)	B(3 on, 3 pause)

	SEQUENCE 
	000 R00	0G0	RGB	00B	R0B	
	0G0	RG0	000	R0B	0GB RGB	...repeat
*/