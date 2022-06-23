#ifndef _SPI_H_
#define _SPI_h_

#include <stdint.h>
#include <stdbool.h>
#include "em_usart.h"

/**
 * @brief Initialize SPI peripheral and GPIO
 * @param speed_hz SPI speed in hz
 * @return None
 */
void spiInit(uint32_t speed_hz);
bool spiTest(void);

/**
 * @brief Initialize SPI speed
 * @param speed_hz SPI speed in hz
 * @return None
 */
void spiInitSpeed(uint32_t speed_hz);
uint8_t spiTransfer(uint8_t value);

#endif /*_SPI_H_*/
