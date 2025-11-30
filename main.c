#include <stdint.h>
#include <stm32f10x.h>

volatile uint8_t timer_flag = 0;

void delay(uint32_t ticks) {
  for (int i = 0; i < ticks; i++) {
    __NOP();
  }
}

void handle_buttons(void) {
  if (!(GPIOC->IDR & GPIO_IDR_IDR14)) {
    delay(10000);
    if (!(GPIOC->IDR & GPIO_IDR_IDR14)) {
      if (TIM2->PSC > 1) {
        TIM2->PSC = TIM2->PSC >> 1;
      }
      while (!(GPIOC->IDR & GPIO_IDR_IDR14))
        ;
    }
  }

  if (!(GPIOC->IDR & GPIO_IDR_IDR15)) {
    delay(10000);
    if (!(GPIOC->IDR & GPIO_IDR_IDR15)) {
      if (TIM2->PSC < 0xFFFF) {
        TIM2->PSC = TIM2->PSC << 1;
      }
      while (!(GPIOC->IDR & GPIO_IDR_IDR15))
        ;
    }
  }
}

void TIM2_IRQHandler(void) {
  if (TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~TIM_SR_UIF;
    timer_flag = 1;
  }
}

int __attribute__((noreturn)) main(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  GPIOC->CRH =
      (GPIOC->CRH & ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13)) | GPIO_CRH_MODE13_0;

  GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14 |
                               GPIO_CRH_CNF15 | GPIO_CRH_MODE15)) |
               GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1;
  GPIOC->ODR |= GPIO_ODR_ODR14 | GPIO_ODR_ODR15;

  TIM2->PSC = 7999;
  TIM2->ARR = 999;
  TIM2->DIER |= TIM_DIER_UIE;
  TIM2->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(TIM2_IRQn);
  while (1) {
    handle_buttons();

    if (timer_flag) {
      timer_flag = 0;
      GPIOC->ODR ^= GPIO_ODR_ODR13;
    }
  }
}
