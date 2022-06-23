#include <stdio.h>
#include "em_gpio.h"
#include "em_i2c.h"
#include "i2cspm.h"
#include "i2c.h"

#include "pin_config.h"
#include "config.h"

#define I2C_PORT I2C0
#define I2C_PORT_INDEX 0

static bool _i2cIsInit = false;
static bool _enable = false;

void i2cInit(void)
{
	DISABLE_HW();
	if (_i2cIsInit)
	{
		return;
	}

	I2CSPM_Init_TypeDef init = I2CSPM_INIT_DEFAULT;
	init.port = I2C_PORT;
	init.i2cMaxFreq = I2C_FREQ_FAST_MAX; // 400Khz
	init.sdaPort = GPIO_I2C0_SDA_PORT;
	init.sdaPin = GPIO_I2C0_SDA_PIN;
	init.sclPort = GPIO_I2C0_SCL_PORT;
	init.sclPin = GPIO_I2C0_SCL_PIN;

	_enable = true;
	_i2cIsInit = true;
	I2CSPM_Init(&init);
	i2cEnable(false);
}

void i2cReset(void)
{
	return;	// don't use
}

bool i2cTest(void)
{
	return _i2cIsInit;
}

void i2cEnable(bool en)
{
	DISABLE_HW();
	assert_true(_i2cIsInit);
	if (en == _enable)
	{
		return;
	}

	_enable = en;
	I2C_Enable(I2C_PORT, _enable);
	if (_enable)
	{
		GPIO_PinModeSet(GPIO_I2C0_SCL_PORT, GPIO_I2C0_SCL_PIN, gpioModeWiredAndPullUp, 1);
		GPIO_PinModeSet(GPIO_I2C0_SDA_PORT, GPIO_I2C0_SDA_PIN, gpioModeWiredAndPullUp, 1);

		/*  In some situations, after a reset during an I2C transfer, the slave
		 device may be left in an unknown state. Send 9 clock pulses to
		 set slave in a defined state. */
		for (uint8_t i = 0; i < 9; i++)
		{
			GPIO_PinOutSet(GPIO_I2C0_SCL_PORT, GPIO_I2C0_SCL_PIN);
			GPIO_PinOutClear(GPIO_I2C0_SDA_PORT, GPIO_I2C0_SDA_PIN);
		}
	}
	else
	{
		GPIO_PinModeSet(GPIO_I2C0_SCL_PORT, GPIO_I2C0_SCL_PIN, gpioModeDisabled, 0);
		GPIO_PinModeSet(GPIO_I2C0_SDA_PORT, GPIO_I2C0_SDA_PIN, gpioModeDisabled, 0);
	}
}

I2C_Result_t i2cWrite(uint8_t slAddr, uint8_t *wdata, uint8_t len)
{
#ifdef DISABLE_ALL_HARDWARE
	return I2C_SUCCESS;
#endif

	assert_true(wdata && _enable && len);
	I2C_TransferSeq_TypeDef seq;

	seq.addr = slAddr << 1;
	seq.flags = I2C_FLAG_WRITE;
	seq.buf[0].data = wdata;
	seq.buf[0].len = len;
	seq.buf[1].data = NULL;
	seq.buf[1].len = 0;

	return (I2CSPM_Transfer(I2C0, &seq) == i2cTransferDone) ? I2C_SUCCESS : I2C_FAILURE;
}

I2C_Result_t i2cRead(uint8_t slAddr, uint8_t dataReg, uint8_t *rdata,
					 uint8_t len)
{
#ifdef DISABLE_ALL_HARDWARE
	return I2C_SUCCESS;
#endif
	assert_true(rdata && _enable && len);
	I2C_TransferSeq_TypeDef seq;

	seq.addr = slAddr << 1;
	seq.flags = I2C_FLAG_WRITE_READ;
	seq.buf[0].data = &dataReg;
	seq.buf[0].len = 1;
	seq.buf[1].data = rdata;
	seq.buf[1].len = len;

	return (I2CSPM_Transfer(I2C0, &seq) == i2cTransferDone) ? I2C_SUCCESS : I2C_FAILURE;
}
