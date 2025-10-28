#include <stdint.h>
#include <stm32f10x.h>

void delay(uint32_t ticks) {
  for (int i = 0; i < ticks; i++) {
    __NOP();
  }
}

uint32_t current_delay = 1000000;
int8_t mult = 0;

void handle_buttons(void) {
  if (!(GPIOC->IDR & GPIO_IDR_IDR14)) {
    delay(10000);
    if (!(GPIOC->IDR & GPIO_IDR_IDR14)) {
      if (mult < 6) {
        mult++;
        current_delay >>= 1;
      }
      while (!(GPIOC->IDR & GPIO_IDR_IDR14))
        ;
    }
  }

  if (!(GPIOC->IDR & GPIO_IDR_IDR15)) {
    delay(10000);
    if (!(GPIOC->IDR & GPIO_IDR_IDR15)) {
      if (mult > -6) {
        mult--;
        current_delay <<= 1;
      }
      while (!(GPIOC->IDR & GPIO_IDR_IDR15))
        ;
    }
  }
}

int __attribute((noreturn)) main(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

  GPIOC->CRH =
      (GPIOC->CRH & ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13)) | GPIO_CRH_MODE13_0;

  GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14 |
                               GPIO_CRH_CNF15 | GPIO_CRH_MODE15)) |
               GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1;
  GPIOC->ODR |= GPIO_ODR_ODR14 | GPIO_ODR_ODR15;

  while (1) {
    handle_buttons();

    GPIOC->ODR ^= GPIO_ODR_ODR13;
    delay(current_delay);
  }
}
