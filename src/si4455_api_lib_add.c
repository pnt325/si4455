#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "em_gpio.h"

#include "si4455_cmd.h"
#include "si4455_prop.h"
#include "si4455_hal.h"
#include "si4455_comm.h"
#include "si4455_api_lib.h"
#include "si4455_api_lib_add.h"

SI4455_CONFIG_return_t SI4455_CONFIG_init(SI4455_HAL_typedef_t* hal, const uint8_t *pSetPropCmd)
{
	uint8_t ezradioCmd[16u];
	ezradio_cmd_reply_t ezradioReply;

	uint8_t col;
	uint8_t response;
	uint8_t numOfBytes;

	while (*pSetPropCmd != 0x00)
	{
		/* Commands structure in the array:
		 * --------------------------------
		 * LEN | <LEN length of data>
		 */

		numOfBytes = *pSetPropCmd++;

		if (numOfBytes > 16u)
		{
			/* Initial configuration of Si4x55 */
			if (EZRADIO_CMD_ID_EZCONFIG_ARRAY_WRITE == *pSetPropCmd)
			{
				if (numOfBytes > 128u)
				{
					/* Number of command bytes exceeds maximal allowable length */
					return EZRADIO_CONFIG_COMMAND_ERROR;
				}

				/* Load array to the device */
				pSetPropCmd++;
				if(SI4455_CONFIG_write_ezconfig_array(hal, numOfBytes - 1, (uint8_t *)pSetPropCmd) != EZRADIO_CONFIG_SUCCESS){
					return EZRADIO_CONFIG_COMMAND_ERROR;
				}

				/* Point to the next command */
				pSetPropCmd += numOfBytes - 1;

				/* Continue command interpreter */
				continue;
			}
			else
			{
				/* Number of command bytes exceeds maximal allowable length */
				return EZRADIO_CONFIG_COMMAND_ERROR;
			}
		}

		for (col = 0u; col < numOfBytes; col++)
		{
			ezradioCmd[col] = *pSetPropCmd;
			pSetPropCmd++;
		}

		if (SI4455_COMM_send_cmd_get_resp(hal, numOfBytes, ezradioCmd, 1, &response) != SI4455_COMM_SUCCESS)
		{
			/* Timeout occured */
			return EZRADIO_CONFIG_CTS_TIMEOUT;
		}

		/* Check response byte of EZCONFIG_CHECK command */
		if (EZRADIO_CMD_ID_EZCONFIG_CHECK == ezradioCmd[0])
		{
			if (response)
			{
				return EZRADIO_CONFIG_COMMAND_ERROR;
			}
		}

		if(hal->nirq_status() == 0)
		{
			/* Get and clear all interrupts.  An error has occured... */
			if(SI4455_CONFIG_get_int_status(hal, 0, 0, 0, &ezradioReply) != EZRADIO_CONFIG_SUCCESS) {
				return EZRADIO_CONFIG_CTS_TIMEOUT;
			}

			if (ezradioReply.GET_INT_STATUS.CHIP_PEND & EZRADIO_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_MASK)
			{
				return EZRADIO_CONFIG_COMMAND_ERROR;
			}
		}
	}

	return EZRADIO_CONFIG_SUCCESS;
}

SI4455_CONFIG_return_t SI4455_CONFIG_write_ezconfig_array(SI4455_HAL_typedef_t* hal, uint8_t numBytes, uint8_t *pEzConfigArray)
{
	if (SI4455_COMM_write_data(hal, EZRADIO_CMD_ID_EZCONFIG_ARRAY_WRITE, 1, numBytes, pEzConfigArray) != SI4455_COMM_SUCCESS) 
	{
		return EZRADIO_CONFIG_COMMAND_ERROR;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

SI4455_CONFIG_return_t SI4455_CONFIG_check_ezconfig(SI4455_HAL_typedef_t* hal, uint16_t checksum, ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[3u];

	ezradioCmd[0] = EZRADIO_CMD_ID_EZCONFIG_CHECK;
	ezradioCmd[1] = (uint8_t)(checksum >> 8u);
	ezradioCmd[2] = (uint8_t)(checksum & 0x00FF);

	/* Do not check CTS after sending the ezconfig array */
	hal->_cts = 1;
	if (SI4455_COMM_send_cmd_get_resp(hal,
									EZRADIO_CMD_ARG_COUNT_EZCONFIG_CHECK,
									ezradioCmd,
									EZRADIO_CMD_REPLY_COUNT_EZCONFIG_CHECK,
									ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->EZCONFIG_CHECK.RESULT = ezradioCmd[0];
	}
	
	return EZRADIO_CONFIG_SUCCESS;
}
