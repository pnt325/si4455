#include <stdint.h>
#include <stdarg.h>
#include "em_gpio.h"
#include "ustimer.h"
#include "si4455_hal.h"
#include "si4455_comm.h"
#include "config.h"

SI4455_COMM_return_t SI4455_COMM_get_resp(SI4455_HAL_typedef_t *hal, uint8_t byteCount, uint8_t *pData)
{
	uint16_t errCnt = 10000;
	uint8_t rxCtsVal;

	while (errCnt)
	{
		hal->set_nsel(0);
		hal->spi_write_byte(0x44);
		hal->spi_read_byte(&rxCtsVal);
		if (rxCtsVal == 0xFF)
		{
			if (byteCount)
			{
				hal->spi_read_data(hal, byteCount, pData);
			}
			hal->set_nsel(1);
			break;
		}
		hal->set_nsel(1);
		errCnt--;
	}

	if (errCnt == 0)
	{
		return SI4455_COMM_TIMEOUT;
	}

	if (rxCtsVal == 0xFF)
	{
		hal->_cts = 1;
	}

	return SI4455_COMM_SUCCESS;
}

SI4455_COMM_return_t SI4455_COMM_send_cmd(SI4455_HAL_typedef_t *hal, uint8_t byteCount, uint8_t *pData)
{
	if (!hal->_cts)
	{
		if (SI4455_COMM_poll_cts(hal) != SI4455_COMM_SUCCESS)
		{
			return SI4455_COMM_TIMEOUT;
		}
	}

	hal->set_nsel(0);
	hal->spi_write_data(hal, byteCount, pData);
	hal->set_nsel(1);
	hal->_cts = 0;

	return SI4455_COMM_SUCCESS;
}

SI4455_COMM_return_t SI4455_COMM_read_data(SI4455_HAL_typedef_t *hal, uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t *pData)
{
	if (pollCts)
	{
		if (!hal->_cts)
		{
			if (SI4455_COMM_poll_cts(hal) != SI4455_COMM_SUCCESS)
			{
				return SI4455_COMM_TIMEOUT;
			}
		}
	}

	hal->set_nsel(0);
	hal->spi_write_byte(cmd);
	hal->spi_read_data(hal, byteCount, pData);
	hal->set_nsel(1);

	hal->_cts = 0;

	return SI4455_COMM_SUCCESS;
}

SI4455_COMM_return_t SI4455_COMM_write_data(SI4455_HAL_typedef_t *hal, uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t *pData)
{
	if (pollCts)
	{
		if (!hal->_cts)
		{
			if (SI4455_COMM_poll_cts(hal) != SI4455_COMM_SUCCESS)
			{
				return SI4455_COMM_TIMEOUT;
			}
		}
	}

	hal->set_nsel(0);
	hal->spi_write_byte(cmd);
	hal->spi_write_data(hal, byteCount, pData);
	hal->set_nsel(1);
	hal->_cts = 0;

	return SI4455_COMM_SUCCESS;
}

SI4455_COMM_return_t SI4455_COMM_poll_cts(SI4455_HAL_typedef_t *hal)
{
	return SI4455_COMM_get_resp(hal, 0, 0);
}

void SI4455_COMM_clear_cts(SI4455_HAL_typedef_t *hal)
{
	hal->_cts = 0;
}

SI4455_COMM_return_t SI4455_COMM_send_cmd_get_resp(SI4455_HAL_typedef_t *hal, uint8_t cmdByteCount, uint8_t *pCmdData, uint8_t respByteCount, uint8_t *pRespData)
{
	if (SI4455_COMM_send_cmd(hal, cmdByteCount, pCmdData) != SI4455_COMM_SUCCESS)
	{
		return SI4455_COMM_TIMEOUT;
	}

	return SI4455_COMM_get_resp(hal, respByteCount, pRespData);
}
