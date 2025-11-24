#include "ssd1306.h"
#include "spi.h"
#include <stm32f10x.h>

uint8_t ssd1306_buffer[SSD1306_BUFFER_SIZE];

static void SSD1306_Delay(uint32_t delay) {
  for (uint32_t i = 0; i < delay; i++) {
    __NOP();
  }
}

void SSD1306_WriteCommand(uint8_t cmd) {
  SSD1306_CS_LOW();
  SSD1306_DC_LOW();
  SPI1_Write(cmd);
  SSD1306_CS_HIGH();
}

void SSD1306_WriteData(uint8_t data) {
  SSD1306_CS_LOW();
  SSD1306_DC_HIGH();
  SPI1_Write(data);
  SSD1306_CS_HIGH();
}

void SSD1306_Init(void) {

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

  GPIOA->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
  GPIOA->CRL |= GPIO_CRL_MODE4;
  SSD1306_CS_HIGH();

  GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
  GPIOB->CRL |= GPIO_CRL_MODE0;

  GPIOB->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
  GPIOB->CRL |= GPIO_CRL_MODE1;

  SPI1_Init();

  SSD1306_RST_HIGH();
  SSD1306_Delay(10000);
  SSD1306_RST_LOW();
  SSD1306_Delay(100000);
  SSD1306_RST_HIGH();
  SSD1306_Delay(100000);

  SSD1306_WriteCommand(SSD1306_DISPLAYOFF);

  SSD1306_WriteCommand(SSD1306_SETDISPLAYCLOCKDIV);
  SSD1306_WriteCommand(0x80);

  SSD1306_WriteCommand(SSD1306_SETMULTIPLEX);
  SSD1306_WriteCommand(0x3F);

  SSD1306_WriteCommand(SSD1306_SETDISPLAYOFFSET);
  SSD1306_WriteCommand(0x00);

  SSD1306_WriteCommand(SSD1306_SETSTARTLINE | 0x00);

  SSD1306_WriteCommand(SSD1306_CHARGEPUMP);
  SSD1306_WriteCommand(0x14);

  SSD1306_WriteCommand(SSD1306_MEMORYMODE);
  SSD1306_WriteCommand(0x00);

  SSD1306_WriteCommand(SSD1306_SEGREMAP | 0x01);

  SSD1306_WriteCommand(SSD1306_COMSCANDEC);

  SSD1306_WriteCommand(SSD1306_SETCOMPINS);
  SSD1306_WriteCommand(0x12);

  SSD1306_WriteCommand(SSD1306_SETCONTRAST);
  SSD1306_WriteCommand(0xCF);

  SSD1306_WriteCommand(SSD1306_SETPRECHARGE);
  SSD1306_WriteCommand(0xF1);

  SSD1306_WriteCommand(SSD1306_SETVCOMDETECT);
  SSD1306_WriteCommand(0x40);

  SSD1306_WriteCommand(SSD1306_DISPLAYALLON_RESUME);

  SSD1306_WriteCommand(SSD1306_NORMALDISPLAY);

  SSD1306_WriteCommand(SSD1306_DISPLAYON);

  SSD1306_Fill(0);
  SSD1306_UpdateScreen();
}

void SSD1306_UpdateScreen(void) {

  SSD1306_WriteCommand(SSD1306_COLUMNADDR);
  SSD1306_WriteCommand(0);
  SSD1306_WriteCommand(127);

  SSD1306_WriteCommand(SSD1306_PAGEADDR);
  SSD1306_WriteCommand(0);
  SSD1306_WriteCommand(7);

  SSD1306_CS_LOW();
  SSD1306_DC_HIGH();

  for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i++) {
    SPI1_Write(ssd1306_buffer[i]);
  }

  SSD1306_CS_HIGH();
}

void SSD1306_Fill(uint8_t color) {
  uint8_t fill_byte = (color) ? 0xFF : 0x00;

  for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i++) {
    ssd1306_buffer[i] = fill_byte;
  }
}

void SSD1306_SetPixel(uint8_t x, uint8_t y, uint8_t color) {
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
    return;
  }

  uint16_t byte_index = x + (y / 8) * SSD1306_WIDTH;
  uint8_t bit_position = y % 8;

  if (color) {
    ssd1306_buffer[byte_index] |= (1 << bit_position);
  } else {
    ssd1306_buffer[byte_index] &= ~(1 << bit_position);
  }
}

void SSD1306_DrawChessboard(uint8_t square_size) {
  SSD1306_Fill(0);

  for (uint8_t y = 0; y < SSD1306_HEIGHT; y++) {
    for (uint8_t x = 0; x < SSD1306_WIDTH; x++) {

      uint8_t col = x / square_size;
      uint8_t row = y / square_size;

      uint8_t color = (col + row) % 2;

      SSD1306_SetPixel(x, y, color);
    }
  }

  SSD1306_UpdateScreen();
}
