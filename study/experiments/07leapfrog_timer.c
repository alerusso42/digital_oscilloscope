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
	TIME_R = TICKS,
	TIME_G = TICKS * 2,
	TIME_B = TICKS * 3,
	PORT_GPIOA = 0,
	PORT_GPIOB = 1,
	MASK_COMPARE = TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF
};

//RGB LED =>	R(A10), G(A3), B(B7)

void	checkPin(volatile uint8_t *flag, int pin, int gpio);

volatile uint8_t	flagR = 0;
volatile uint8_t	flagG = 0;
volatile uint8_t	flagB = 0;

int main(void)
{
	RCC->AHB1ENR |= 0b11;
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->PSC = PRESCALE - 1;
	TIM1->ARR = UINT16_MAX;
	TIM1->DIER = TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE;//enable update interrupt
	TIM1->CNT = 0;//start val
	GPIOA->MODER &= ~(0b11 << (2 * PIN_R));
	GPIOA->MODER &= ~(0b11 << (2 * PIN_G));
	GPIOB->MODER &= ~(0b11 << (2 * PIN_B));
	GPIOA->MODER |= (0b01 << (2 * PIN_R));
	GPIOA->MODER |= (0b01 << (2 * PIN_G));
	GPIOB->MODER |= (0b01 << (2 * PIN_B));
	GPIOA->ODR &= ~(1 << PIN_R);
	GPIOA->ODR &= ~(1 << PIN_G);
	GPIOB->ODR &= ~(1 << PIN_B);
	TIM1->CCR1 = TIME_R;
	TIM1->CCR2 = TIME_G;
	TIM1->CCR3 = TIME_B;
	TIM1->EGR  = TIM_EGR_UG;//IMPORTANT: force timer data update
	TIM1->SR = ~MASK_COMPARE;
	TIM1->CR1 |= TIM_CR1_CEN;//enable timer
	NVIC_EnableIRQ(TIM1_CC_IRQn);
	while (1)
	{
		checkPin(&flagR, PIN_R, PORT_GPIOA);
		checkPin(&flagG, PIN_G, PORT_GPIOA);
		checkPin(&flagB, PIN_B, PORT_GPIOB);
	}
}

void	checkPin(volatile uint8_t *shared_flag, int pin, int gpio)
{
	uint8_t	flag;

	NVIC_DisableIRQ(TIM1_CC_IRQn);
	flag = *shared_flag;
	NVIC_EnableIRQ(TIM1_CC_IRQn);
	if (!flag)
		return ;
	if (gpio == PORT_GPIOA)
		GPIOA->ODR ^= (1 << pin);
	else if (gpio == PORT_GPIOB)
		GPIOB->ODR ^= (1 << pin);
	else
		return ;
	NVIC_DisableIRQ(TIM1_CC_IRQn);
	*shared_flag = 0;
	NVIC_EnableIRQ(TIM1_CC_IRQn);
}

void TIM1_CC_IRQHandler(void)
{
	if (!(TIM1->SR & MASK_COMPARE))
		return ;
	if (TIM1->SR & TIM_SR_CC1IF)
	{
		TIM1->CCR1 += TIME_R;
		flagR = 1;
		TIM1->SR = ~TIM_SR_CC1IF;//interrupt off
	}
	if (TIM1->SR & TIM_SR_CC2IF)
	{
		TIM1->CCR2 += TIME_G;
		flagG = 1;
		TIM1->SR = ~TIM_SR_CC2IF;//interrupt off
	}
	if (TIM1->SR & TIM_SR_CC3IF)
	{
		TIM1->CCR3 += TIME_B;
		flagB = 1;
		TIM1->SR = ~TIM_SR_CC3IF;//interrupt off
	}
}

/*
	R(1 on, 1 stop)	G(2 on, 2 pause)	B(3 on, 3 pause)

	SEQUENCE 
	000 R00	0G0	RGB	00B	R0B	
	0G0	RG0	000	R0B	0GB RGB	...repeat
*/