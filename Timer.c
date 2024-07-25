#include "Timer.h"
#include "stm32f10x.h"

volatile uint16_t ADCData;
volatile uint8_t ADCstatus;

void Timer3A_Init(uint32_t period, uint32_t priority) {
    // B?t clock cho Timer 3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // T?t Timer 3 tru?c khi c?u h?nh
    TIM3->CR1 &= ~TIM_CR1_CEN;

    // C?u h?nh Timer 3
    TIM3->CR1 = 0x00000000; // ??t CR1 v? gi? tr? m?c d?nh
    TIM3->CR1 &= ~TIM_CR1_DIR; // Thi?t l?p ch? d? d?m l?n (upcounting)
    TIM3->PSC = 7200 - 1; // Thi?t l?p Prescaler
    TIM3->ARR = period - 1; // Thi?t l?p gi? tr? Period

    // X?a c? ng?t
    TIM3->SR &= ~TIM_SR_UIF;

    // B?t ng?t cho Timer 3
    TIM3->DIER |= TIM_DIER_UIE;

    // Thi?t l?p uu ti?n ng?t
    NVIC_SetPriority(TIM3_IRQn, priority);

    // B?t ng?t Timer 3
    NVIC_EnableIRQ(TIM3_IRQn);

    // B?t Timer 3
    TIM3->CR1 |= TIM_CR1_CEN;
}
void TIM3_IRQHandler(void){
    if (TIM3->SR & TIM_SR_UIF) {
        // Clear update interrupt flag
        TIM3->SR &= ~TIM_SR_UIF;

        // Read ADC value
//        ADCData = ADC_In();
				// Start ADC conversion
			  ADC1->CR2 |= ADC_CR2_SWSTART;
        ADCstatus = 1;

        // Toggle PC13 (for testing purposes)
        GPIOC->ODR ^= 1 << 13;
    }
}

