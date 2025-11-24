#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void SPI1_Init(void);

void SPI1_Write(uint8_t data);

uint8_t SPI1_Read(void);

uint8_t SPI1_Transfer(uint8_t data);

#endif
