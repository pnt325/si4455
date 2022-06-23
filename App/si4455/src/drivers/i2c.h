#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    I2C_SUCCESS = 0,
    I2C_FAILURE
}I2C_Result_t;

void i2cInit(void);
void i2cReset(void);
bool i2cTest(void);
void i2cEnable(bool en);
I2C_Result_t i2cWrite(uint8_t slAddr, uint8_t* wdata, uint8_t len);
I2C_Result_t i2cRead(uint8_t slAddr, uint8_t dataReg, uint8_t* rdata, uint8_t len);

/**
 * read
 * write
 * init
 * deinit
 * enable
 * disable
 */

#endif /*_I2C_H_*/
