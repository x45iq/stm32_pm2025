#include "ssd1306.h"
#include <stdint.h>
#include <stm32f10x.h>

void delay(uint32_t ticks) {
  for (uint32_t i = 0; i < ticks; i++) {
    __NOP();
  }
}

uint8_t chessboard_sizes[] = {4, 8, 16, 32};
uint8_t current_size_index = 1;

void handle_buttons(void) {
  if (!(GPIOC->IDR & GPIO_IDR_IDR14)) {
    delay(50000);
    if (!(GPIOC->IDR & GPIO_IDR_IDR14)) {
      current_size_index++;
      if (current_size_index >= sizeof(chessboard_sizes)) {
        current_size_index = 0;
      }

      SSD1306_DrawChessboard(chessboard_sizes[current_size_index]);

      GPIOC->ODR &= ~GPIO_ODR_ODR13;
      delay(100000);
      GPIOC->ODR |= GPIO_ODR_ODR13;

      while (!(GPIOC->IDR & GPIO_IDR_IDR14))
        ;
      delay(50000);
    }
  }

  if (!(GPIOC->IDR & GPIO_IDR_IDR15)) {
    delay(50000);
    if (!(GPIOC->IDR & GPIO_IDR_IDR15)) {
      if (current_size_index == 0) {
        current_size_index = sizeof(chessboard_sizes) - 1;
      } else {
        current_size_index--;
      }

      SSD1306_DrawChessboard(chessboard_sizes[current_size_index]);

      GPIOC->ODR &= ~GPIO_ODR_ODR13;
      delay(100000);
      GPIOC->ODR |= GPIO_ODR_ODR13;

      while (!(GPIOC->IDR & GPIO_IDR_IDR15))
        ;
      delay(50000);
    }
  }
}

int __attribute__((noreturn)) main(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

  GPIOC->CRH =
      (GPIOC->CRH & ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13)) | GPIO_CRH_MODE13_0;
  GPIOC->ODR |= GPIO_ODR_ODR13;

  GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14 |
                               GPIO_CRH_CNF15 | GPIO_CRH_MODE15)) |
               GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1;
  GPIOC->ODR |= GPIO_ODR_ODR14 | GPIO_ODR_ODR15;

  SSD1306_Init();

  for (uint8_t i = 0; i < 3; i++) {
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    delay(200000);
  }
  GPIOC->ODR |= GPIO_ODR_ODR13;

  SSD1306_DrawChessboard(chessboard_sizes[current_size_index]);

  while (1) {
    handle_buttons();

    delay(10000);
  }
}
