#include <stdint.h>
#include <stdio.h>

#include "si4455_cmd.h"
#include "si4455_prop.h"
#include "si4455_comm.h"
#include "si4455_hal.h"
#include "si4455_api_lib.h"

SI4455_CONFIG_return_t SI4455_CONFIG_reset(SI4455_HAL_typedef_t *hal)
{
	hal->assert_shutdown();
	hal->delay_ms(1);
	hal->deassert_shutdown();
	hal->delay_ms(1);
	SI4455_COMM_clear_cts(hal);
	return EZRADIO_CONFIG_SUCCESS;
}

SI4455_CONFIG_return_t SI4455_CONFIG_power_up(SI4455_HAL_typedef_t *hal, uint8_t boot_options, uint8_t xtal_options, uint32_t xo_freq)
{
	uint8_t ezradioCmd[7u];
	ezradioCmd[0] = EZRADIO_CMD_ID_POWER_UP;
	ezradioCmd[1] = boot_options;
	ezradioCmd[2] = xtal_options;
	ezradioCmd[3] = (uint8_t)(xo_freq >> 24);
	ezradioCmd[4] = (uint8_t)(xo_freq >> 16);
	ezradioCmd[5] = (uint8_t)(xo_freq >> 8);
	ezradioCmd[6] = (uint8_t)(xo_freq);
	if (SI4455_COMM_send_cmd(hal, EZRADIO_CMD_ARG_COUNT_POWER_UP, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

SI4455_CONFIG_return_t SI4455_CONFIG_part_info(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply)
{
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_PART_INFO];
	ezradioCmd[0] = EZRADIO_CMD_ID_PART_INFO;
	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_PART_INFO,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_PART_INFO,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->PART_INFO.CHIPREV = ezradioCmd[0];
		ezradioReply->PART_INFO.PART = (((uint16_t)ezradioCmd[1] << 8) & 0xFF00) + ((uint16_t)ezradioCmd[2] & 0x00FF);
		ezradioReply->PART_INFO.PBUILD = ezradioCmd[3];
		ezradioReply->PART_INFO.ID = (((uint16_t)ezradioCmd[4] << 8) & 0xFF00) + ((uint16_t)ezradioCmd[5] & 0x00FF);
		ezradioReply->PART_INFO.CUSTOMER = ezradioCmd[6];
		ezradioReply->PART_INFO.ROMID = ezradioCmd[7];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/** Sends START_TX command to the radio.
 *
 * @param[in] channel   Channel number.
 * @param[in] condition Start TX condition.
 * @param[in] tx_len    Payload length (exclude the PH generated CRC).
 */
SI4455_CONFIG_return_t SI4455_CONFIG_start_tx(SI4455_HAL_typedef_t *hal, uint8_t channel, uint8_t condition, uint16_t tx_len)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[7u];

	ezradioCmd[0] = EZRADIO_CMD_ID_START_TX;
	ezradioCmd[1] = channel;
	ezradioCmd[2] = condition;
	ezradioCmd[3] = (uint8_t)(tx_len >> 8);
	ezradioCmd[4] = (uint8_t)(tx_len);
	ezradioCmd[5] = 0x00;

	// Don't repeat the packet,
	// ie. transmit the packet only once
	ezradioCmd[6] = 0x00;

	if (SI4455_COMM_send_cmd(hal, EZRADIO_CMD_ARG_COUNT_START_TX, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Sends START_RX command to the radio.
 *
 * @param[in] channel     Channel number.
 * @param[in] condition   Start RX condition.
 * @param[in] rx_len      Payload length (exclude the PH generated CRC).
 * @param[in] next_state1 Next state when Preamble Timeout occurs.
 * @param[in] next_state2 Next state when a valid packet received.
 * @param[in] next_state3 Next state when invalid packet received (e.g. CRC error).
 */
SI4455_CONFIG_return_t SI4455_CONFIG_start_rx(SI4455_HAL_typedef_t *hal,
											  uint8_t channel,
											  uint8_t condition,
											  uint16_t rx_len,
											  uint8_t next_state1,
											  uint8_t next_state2,
											  uint8_t next_state3)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[8u];

	ezradioCmd[0] = EZRADIO_CMD_ID_START_RX;
	ezradioCmd[1] = channel;
	ezradioCmd[2] = condition;
	ezradioCmd[3] = (uint8_t)(rx_len >> 8);
	ezradioCmd[4] = (uint8_t)(rx_len);
	ezradioCmd[5] = next_state1;
	ezradioCmd[6] = next_state2;
	ezradioCmd[7] = next_state3;

	if (SI4455_COMM_send_cmd(hal, EZRADIO_CMD_ARG_COUNT_START_RX, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Get the Interrupt status/pending flags form the radio and clear flags if requested.
 *
 * @param[in] ph_clr_pend     Packet Handler pending flags clear.
 * @param[in] modem_clr_pend  Modem Status pending flags clear.
 * @param[in] chip_clr_pend   Chip State pending flags clear.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_int_status(SI4455_HAL_typedef_t *hal,
													uint8_t ph_clr_pend,
													uint8_t modem_clr_pend,
													uint8_t chip_clr_pend,
													ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_INT_STATUS];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_INT_STATUS;
	ezradioCmd[1] = ph_clr_pend;
	ezradioCmd[2] = modem_clr_pend;
	ezradioCmd[3] = chip_clr_pend;

	if (SI4455_COMM_send_cmd_get_resp(hal, EZRADIO_CMD_ARG_COUNT_GET_INT_STATUS,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GET_INT_STATUS, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_INT_STATUS.INT_PEND = ezradioCmd[0];
		ezradioReply->GET_INT_STATUS.INT_STATUS = ezradioCmd[1];
		ezradioReply->GET_INT_STATUS.PH_PEND = ezradioCmd[2];
		ezradioReply->GET_INT_STATUS.PH_STATUS = ezradioCmd[3];
		ezradioReply->GET_INT_STATUS.MODEM_PEND = ezradioCmd[4];
		ezradioReply->GET_INT_STATUS.MODEM_STATUS = ezradioCmd[5];
		ezradioReply->GET_INT_STATUS.CHIP_PEND = ezradioCmd[6];
		ezradioReply->GET_INT_STATUS.CHIP_STATUS = ezradioCmd[7];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Send GPIO pin config command to the radio and reads the answer into
 * ezradioReply union.
 *
 * @param[in] gpio0       GPIO0 configuration.
 * @param[in] gpio1       GPIO1 configuration.
 * @param[in] gpio2       GPIO2 configuration.
 * @param[in] gpio3       GPIO3 configuration.
 * @param[in] nirq        NIRQ configuration.
 * @param[in] sdo         SDO configuration.
 * @param[in] gen_config  General pin configuration.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_gpio_pin_cfg(SI4455_HAL_typedef_t *hal,
												  uint8_t gpio0,
												  uint8_t gpio1,
												  uint8_t gpio2,
												  uint8_t gpio3,
												  uint8_t nirq,
												  uint8_t sdo,
												  uint8_t gen_config,
												  ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_ARG_COUNT_GPIO_PIN_CFG];

	ezradioCmd[0] = EZRADIO_CMD_ID_GPIO_PIN_CFG;
	ezradioCmd[1] = gpio0;
	ezradioCmd[2] = gpio1;
	ezradioCmd[3] = gpio2;
	ezradioCmd[4] = gpio3;
	ezradioCmd[5] = nirq;
	ezradioCmd[6] = sdo;
	ezradioCmd[7] = gen_config;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_GPIO_PIN_CFG,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GPIO_PIN_CFG,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GPIO_PIN_CFG.gpIO[0] = ezradioCmd[0];
		ezradioReply->GPIO_PIN_CFG.gpIO[1] = ezradioCmd[1];
		ezradioReply->GPIO_PIN_CFG.gpIO[2] = ezradioCmd[2];
		ezradioReply->GPIO_PIN_CFG.gpIO[3] = ezradioCmd[3];
		ezradioReply->GPIO_PIN_CFG.NIRQ = ezradioCmd[4];
		ezradioReply->GPIO_PIN_CFG.SDO = ezradioCmd[5];
		ezradioReply->GPIO_PIN_CFG.GEN_CONFIG = ezradioCmd[6];
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Send SET_PROPERTY command to the radio.
 *
 * @param[in] group       Property group.
 * @param[in] num_props   Number of property to be set. The properties must be in ascending order
 *                    in their sub-property aspect. Max. 12 properties can be set in one command.
 * @param[in] start_prop  Start sub-property address.
 * @param[in] ...  Variable number of properties to be set.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_set_property(SI4455_HAL_typedef_t *hal,
												  uint8_t group,
												  uint8_t num_props,
												  uint8_t start_prop,
												  uint8_t *datas)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[16u];
	uint8_t cmdIndex;

	ezradioCmd[0] = EZRADIO_CMD_ID_SET_PROPERTY;
	ezradioCmd[1] = group;
	ezradioCmd[2] = num_props;
	ezradioCmd[3] = start_prop;
	cmdIndex = 4;
	for (uint8_t i = 0; i < num_props; i++)
	{
		ezradioCmd[cmdIndex++] = datas[i];
	}

	if (SI4455_COMM_send_cmd(hal, cmdIndex, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Issue a change state command to the radio.
 *
 * @param[in] next_state1 Next state.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_change_state(SI4455_HAL_typedef_t *hal, uint8_t next_state1)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[2u];

	ezradioCmd[0] = EZRADIO_CMD_ID_CHANGE_STATE;
	ezradioCmd[1] = next_state1;

	if (SI4455_COMM_send_cmd(hal,
							 EZRADIO_CMD_ARG_COUNT_CHANGE_STATE,
							 ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/* Extended driver support functions */

/**
 * Sends NOP command to the radio. Can be used to maintain SPI communication.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_nop(SI4455_HAL_typedef_t *hal)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[1u];

	ezradioCmd[0] = EZRADIO_CMD_ID_NOP;

	if (SI4455_COMM_send_cmd(hal, EZRADIO_CMD_ARG_COUNT_NOP, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Send the FIFO_INFO command to the radio. Optionally resets the TX/RX FIFO. Reads the radio response back
 * into ezradioReply.
 *
 * @param[in] fifo  RX/TX FIFO reset flags.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_fifo_info(SI4455_HAL_typedef_t *hal,
											   uint8_t fifo,
											   ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_FIFO_INFO];

	ezradioCmd[0] = EZRADIO_CMD_ID_FIFO_INFO;
	ezradioCmd[1] = fifo;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_FIFO_INFO,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_FIFO_INFO,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->FIFO_INFO.RX_FIFO_COUNT = ezradioCmd[0];
		ezradioReply->FIFO_INFO.TX_FIFO_SPACE = ezradioCmd[1];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * The function can be used to load data into TX FIFO.
 *
 * @param[in] numBytes  Data length to be load.
 * @param[in] pTxData   Pointer to the data (uint8_t*).
 */
SI4455_CONFIG_return_t SI4455_CONFIG_write_tx_fifo(SI4455_HAL_typedef_t *hal,
												   uint8_t numBytes,
												   uint8_t *pTxData)
{
	if (SI4455_COMM_write_data(hal,
							   EZRADIO_CMD_ID_WRITE_TX_FIFO,
							   0,
							   numBytes,
							   pTxData) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Reads the RX FIFO content from the radio.
 *
 * @param[in] numBytes  Data length to be read.
 * @param[in] pRxData   Pointer to the buffer location.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_read_rx_fifo(SI4455_HAL_typedef_t *hal,
												  uint8_t numBytes,
												  uint8_t *pRxData)
{
	if (SI4455_COMM_read_data(hal,
							  EZRADIO_CMD_ID_READ_RX_FIFO,
							  0,
							  numBytes,
							  pRxData) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Get property values from the radio. Reads them into ezradioReply union.
 *
 * @param[in] group       Property group number.
 * @param[in] num_props   Number of properties to be read.
 * @param[in] start_prop  Starting sub-property number.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_property(SI4455_HAL_typedef_t *hal,
												  uint8_t group,
												  uint8_t num_props,
												  uint8_t start_prop,
												  ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[16u];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_PROPERTY;
	ezradioCmd[1] = group;
	ezradioCmd[2] = num_props;
	ezradioCmd[3] = start_prop;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_GET_PROPERTY,
									  ezradioCmd,
									  ezradioCmd[2],
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_PROPERTY.DATA[0] = ezradioCmd[0];
		ezradioReply->GET_PROPERTY.DATA[1] = ezradioCmd[1];
		ezradioReply->GET_PROPERTY.DATA[2] = ezradioCmd[2];
		ezradioReply->GET_PROPERTY.DATA[3] = ezradioCmd[3];
		ezradioReply->GET_PROPERTY.DATA[4] = ezradioCmd[4];
		ezradioReply->GET_PROPERTY.DATA[5] = ezradioCmd[5];
		ezradioReply->GET_PROPERTY.DATA[6] = ezradioCmd[6];
		ezradioReply->GET_PROPERTY.DATA[7] = ezradioCmd[7];
		ezradioReply->GET_PROPERTY.DATA[8] = ezradioCmd[8];
		ezradioReply->GET_PROPERTY.DATA[9] = ezradioCmd[9];
		ezradioReply->GET_PROPERTY.DATA[10] = ezradioCmd[10];
		ezradioReply->GET_PROPERTY.DATA[11] = ezradioCmd[11];
		ezradioReply->GET_PROPERTY.DATA[12] = ezradioCmd[12];
		ezradioReply->GET_PROPERTY.DATA[13] = ezradioCmd[13];
		ezradioReply->GET_PROPERTY.DATA[14] = ezradioCmd[14];
		ezradioReply->GET_PROPERTY.DATA[15] = ezradioCmd[15];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/* Full driver support functions */

/**
 * Sends the FUNC_INFO command to the radio, then reads the resonse into ezradioReply union.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_func_info(SI4455_HAL_typedef_t *hal,
											   ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_FUNC_INFO];

	ezradioCmd[0] = EZRADIO_CMD_ID_FUNC_INFO;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_FUNC_INFO,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_FUNC_INFO,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->FUNC_INFO.REVEXT = ezradioCmd[0];
		ezradioReply->FUNC_INFO.REVBRANCH = ezradioCmd[1];
		ezradioReply->FUNC_INFO.REVINT = ezradioCmd[2];
		ezradioReply->FUNC_INFO.FUNC = ezradioCmd[5];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Reads the Fast Response Registers starting with A register into ezradioReply union.
 *
 * @param[in] respByteCount Number of Fast Response Registers to be read.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_frr_a_read(SI4455_HAL_typedef_t *hal,
												uint8_t respByteCount,
												ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[4u];

	if (SI4455_COMM_read_data(hal,
							  EZRADIO_CMD_ID_FRR_A_READ,
							  0,
							  respByteCount,
							  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->FRR_A_READ.FRR_A_VALUE = ezradioCmd[0];
		ezradioReply->FRR_A_READ.FRR_B_VALUE = ezradioCmd[1];
		ezradioReply->FRR_A_READ.FRR_C_VALUE = ezradioCmd[2];
		ezradioReply->FRR_A_READ.FRR_D_VALUE = ezradioCmd[3];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Reads the Fast Response Registers starting with B register into ezradioReply union.
 *
 * @param[in] respByteCount Number of Fast Response Registers to be read.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_frr_b_read(SI4455_HAL_typedef_t *hal,
												uint8_t respByteCount,
												ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[4u];

	if (SI4455_COMM_read_data(hal,
							  EZRADIO_CMD_ID_FRR_B_READ,
							  0,
							  respByteCount,
							  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->FRR_B_READ.FRR_B_VALUE = ezradioCmd[0];
		ezradioReply->FRR_B_READ.FRR_C_VALUE = ezradioCmd[1];
		ezradioReply->FRR_B_READ.FRR_D_VALUE = ezradioCmd[2];
		ezradioReply->FRR_B_READ.FRR_A_VALUE = ezradioCmd[3];
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Reads the Fast Response Registers starting with C register into ezradioReply union.
 *
 * @param[in] respByteCount Number of Fast Response Registers to be read.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_frr_c_read(SI4455_HAL_typedef_t *hal,
												uint8_t respByteCount,
												ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[4u];

	if (SI4455_COMM_read_data(hal,
							  EZRADIO_CMD_ID_FRR_C_READ,
							  0, respByteCount,
							  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->FRR_C_READ.FRR_C_VALUE = ezradioCmd[0];
		ezradioReply->FRR_C_READ.FRR_D_VALUE = ezradioCmd[1];
		ezradioReply->FRR_C_READ.FRR_A_VALUE = ezradioCmd[2];
		ezradioReply->FRR_C_READ.FRR_B_VALUE = ezradioCmd[3];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Reads the Fast Response Registers starting with D register into ezradioReply union.
 *
 * @param[in] respByteCount Number of Fast Response Registers to be read.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_frr_d_read(SI4455_HAL_typedef_t *hal,
												uint8_t respByteCount,
												ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[4u];

	if (SI4455_COMM_read_data(hal,
							  EZRADIO_CMD_ID_FRR_D_READ,
							  0,
							  respByteCount,
							  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->FRR_D_READ.FRR_D_VALUE = ezradioCmd[0];
		ezradioReply->FRR_D_READ.FRR_A_VALUE = ezradioCmd[1];
		ezradioReply->FRR_D_READ.FRR_B_VALUE = ezradioCmd[2];
		ezradioReply->FRR_D_READ.FRR_C_VALUE = ezradioCmd[3];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Requests the current state of the device and lists pending TX and RX requests.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_request_device_state(SI4455_HAL_typedef_t *hal,
														  ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_REQUEST_DEVICE_STATE];

	ezradioCmd[0] = EZRADIO_CMD_ID_REQUEST_DEVICE_STATE;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_REQUEST_DEVICE_STATE,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_REQUEST_DEVICE_STATE,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->REQUEST_DEVICE_STATE.CURR_STATE = ezradioCmd[0];
		ezradioReply->REQUEST_DEVICE_STATE.CURRENT_CHANNEL = ezradioCmd[1];
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Reads the command buffer
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_read_cmd_buff(SI4455_HAL_typedef_t *hal,
												   ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_READ_CMD_BUFF];

	ezradioCmd[0] = EZRADIO_CMD_ID_READ_CMD_BUFF;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_READ_CMD_BUFF,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_READ_CMD_BUFF,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->READ_CMD_BUFF.BYTE[0] = ezradioCmd[0];
		ezradioReply->READ_CMD_BUFF.BYTE[1] = ezradioCmd[1];
		ezradioReply->READ_CMD_BUFF.BYTE[2] = ezradioCmd[2];
		ezradioReply->READ_CMD_BUFF.BYTE[3] = ezradioCmd[3];
		ezradioReply->READ_CMD_BUFF.BYTE[4] = ezradioCmd[4];
		ezradioReply->READ_CMD_BUFF.BYTE[5] = ezradioCmd[5];
		ezradioReply->READ_CMD_BUFF.BYTE[6] = ezradioCmd[6];
		ezradioReply->READ_CMD_BUFF.BYTE[7] = ezradioCmd[7];
		ezradioReply->READ_CMD_BUFF.BYTE[8] = ezradioCmd[8];
		ezradioReply->READ_CMD_BUFF.BYTE[9] = ezradioCmd[9];
		ezradioReply->READ_CMD_BUFF.BYTE[10] = ezradioCmd[10];
		ezradioReply->READ_CMD_BUFF.BYTE[11] = ezradioCmd[11];
		ezradioReply->READ_CMD_BUFF.BYTE[12] = ezradioCmd[12];
		ezradioReply->READ_CMD_BUFF.BYTE[13] = ezradioCmd[13];
		ezradioReply->READ_CMD_BUFF.BYTE[14] = ezradioCmd[14];
		ezradioReply->READ_CMD_BUFF.BYTE[15] = ezradioCmd[15];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Gets the Packet Handler status flags. Optionally clears them.
 *
 * @param[in] ph_clr_pend Flags to clear.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_ph_status(SI4455_HAL_typedef_t *hal,
												   uint8_t ph_clr_pend,
												   ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_PH_STATUS];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_PH_STATUS;
	ezradioCmd[1] = ph_clr_pend;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_GET_PH_STATUS,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GET_PH_STATUS,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_PH_STATUS.PH_PEND = ezradioCmd[0];
		ezradioReply->GET_PH_STATUS.PH_STATUS = ezradioCmd[1];
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Gets the Modem status flags. Optionally clears them.
 *
 * @param[in] modem_clr_pend Flags to clear.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_modem_status(SI4455_HAL_typedef_t *hal,
													  uint8_t modem_clr_pend,
													  ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_MODEM_STATUS];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_MODEM_STATUS;
	ezradioCmd[1] = modem_clr_pend;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_GET_MODEM_STATUS,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GET_MODEM_STATUS,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_MODEM_STATUS.MODEM_PEND = ezradioCmd[0];
		ezradioReply->GET_MODEM_STATUS.MODEM_STATUS = ezradioCmd[1];
		ezradioReply->GET_MODEM_STATUS.CURR_RSSI = ezradioCmd[2];
		ezradioReply->GET_MODEM_STATUS.LATCH_RSSI = ezradioCmd[3];
		ezradioReply->GET_MODEM_STATUS.ANT1_RSSI = ezradioCmd[4];
		ezradioReply->GET_MODEM_STATUS.ANT2_RSSI = ezradioCmd[5];
		ezradioReply->GET_MODEM_STATUS.AFC_FREQ_OFFSET = ((uint16_t)ezradioCmd[6] << 8) & 0xFF00;
		ezradioReply->GET_MODEM_STATUS.AFC_FREQ_OFFSET |= (uint16_t)ezradioCmd[7] & 0x00FF;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Gets the Chip status flags. Optionally clears them.
 *
 * @param[in] chip_clr_pend Flags to clear.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_chip_status(SI4455_HAL_typedef_t *hal,
													 uint8_t chip_clr_pend,
													 ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_CHIP_STATUS];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_CHIP_STATUS;
	ezradioCmd[1] = chip_clr_pend;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_GET_CHIP_STATUS,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GET_CHIP_STATUS,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_CHIP_STATUS.CHIP_PEND = ezradioCmd[0];
		ezradioReply->GET_CHIP_STATUS.CHIP_STATUS = ezradioCmd[1];
		ezradioReply->GET_CHIP_STATUS.CMD_ERR_STATUS = ezradioCmd[2];
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Receives information from the radio of the current packet. Optionally can be used to modify
 * the Packet Handler properties during packet reception.
 *
 * @param[in] field_number_mask Packet Field number mask value.
 * @param[in] len               Length value.
 * @param[in] diff_len          Difference length.
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_packet_info(SI4455_HAL_typedef_t *hal,
													 uint8_t field_number_mask,
													 uint16_t len,
													 int16_t diff_len,
													 ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_ARG_COUNT_PACKET_INFO];

	ezradioCmd[0] = EZRADIO_CMD_ID_PACKET_INFO;
	ezradioCmd[1] = field_number_mask;
	ezradioCmd[2] = (uint8_t)(len >> 8);
	ezradioCmd[3] = (uint8_t)(len);
	// the different of the byte, althrough it is signed, but to command hander
	// it can treat it as unsigned
	ezradioCmd[4] = (uint8_t)((uint16_t)diff_len >> 8);
	ezradioCmd[5] = (uint8_t)(diff_len);

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  EZRADIO_CMD_ARG_COUNT_PACKET_INFO,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_PACKET_INFO,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->PACKET_INFO.LENGTH = ((uint16_t)ezradioCmd[0] << 8) & 0xFF00;
		ezradioReply->PACKET_INFO.LENGTH |= (uint16_t)ezradioCmd[1] & 0x00FF;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/*
 * Faster versions of the above commands.
 */

/** Sends START_TX command ID to the radio with no input parameters
 *
 */
SI4455_CONFIG_return_t SI4455_CONFIG_start_tx_fast(SI4455_HAL_typedef_t *hal)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[1u];

	ezradioCmd[0] = EZRADIO_CMD_ID_START_TX;

	if (SI4455_COMM_send_cmd(hal, 1u, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Sends START_RX command ID to the radio with no input parameters
 *
 */
SI4455_CONFIG_return_t SI4455_CONFIG_start_rx_fast(SI4455_HAL_typedef_t *hal)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[1u];

	ezradioCmd[0] = EZRADIO_CMD_ID_START_RX;

	if (SI4455_COMM_send_cmd(hal, 1u, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_SUCCESS;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Clear all Interrupt status/pending flags. Does NOT read back interrupt flags
 *
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_int_status_fast_clear(
	SI4455_HAL_typedef_t *hal)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[1u];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_INT_STATUS;

	if (SI4455_COMM_send_cmd(hal, 1u, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Clear and read all Interrupt status/pending flags
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_int_status_fast_clear_read(SI4455_HAL_typedef_t *hal,
																	ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_INT_STATUS];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_INT_STATUS;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  1u,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GET_INT_STATUS,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_INT_STATUS.INT_PEND = ezradioCmd[0];
		ezradioReply->GET_INT_STATUS.INT_STATUS = ezradioCmd[1];
		ezradioReply->GET_INT_STATUS.PH_PEND = ezradioCmd[2];
		ezradioReply->GET_INT_STATUS.PH_STATUS = ezradioCmd[3];
		ezradioReply->GET_INT_STATUS.MODEM_PEND = ezradioCmd[4];
		ezradioReply->GET_INT_STATUS.MODEM_STATUS = ezradioCmd[5];
		ezradioReply->GET_INT_STATUS.CHIP_PEND = ezradioCmd[6];
		ezradioReply->GET_INT_STATUS.CHIP_STATUS = ezradioCmd[7];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Reads back current GPIO pin configuration. Does NOT configure GPIO pins
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_gpio_pin_cfg_fast(SI4455_HAL_typedef_t *hal,
													   ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GPIO_PIN_CFG];

	ezradioCmd[0] = EZRADIO_CMD_ID_GPIO_PIN_CFG;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  1u,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GPIO_PIN_CFG,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GPIO_PIN_CFG.gpIO[0] = ezradioCmd[0];
		ezradioReply->GPIO_PIN_CFG.gpIO[1] = ezradioCmd[1];
		ezradioReply->GPIO_PIN_CFG.gpIO[2] = ezradioCmd[2];
		ezradioReply->GPIO_PIN_CFG.gpIO[3] = ezradioCmd[3];
		ezradioReply->GPIO_PIN_CFG.NIRQ = ezradioCmd[4];
		ezradioReply->GPIO_PIN_CFG.SDO = ezradioCmd[5];
		ezradioReply->GPIO_PIN_CFG.GEN_CONFIG = ezradioCmd[6];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Clear all Packet Handler status flags. Does NOT read back interrupt flags
 *
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_ph_status_fast_clear(SI4455_HAL_typedef_t *hal)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[2u];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_PH_STATUS;
	ezradioCmd[1] = 0;

	if (SI4455_COMM_send_cmd(hal, 2u, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Clear and read all Packet Handler status flags.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_ph_status_fast_clear_read(SI4455_HAL_typedef_t *hal,
																   ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_PH_STATUS];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_PH_STATUS;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  1u,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GET_PH_STATUS,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_PH_STATUS.PH_PEND = ezradioCmd[0];
		ezradioReply->GET_PH_STATUS.PH_STATUS = ezradioCmd[1];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Clear all Modem status flags. Does NOT read back interrupt flags
 *
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_modem_status_fast_clear(
	SI4455_HAL_typedef_t *hal)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[2u];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_MODEM_STATUS;
	ezradioCmd[1] = 0;

	if (SI4455_COMM_send_cmd(hal, 2u, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Clear and read all Modem status flags.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_modem_status_fast_clear_read(
	SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply)
{
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_MODEM_STATUS];
	ezradioCmd[0] = EZRADIO_CMD_ID_GET_MODEM_STATUS;
	if (SI4455_COMM_send_cmd_get_resp(hal,
									  1u,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GET_MODEM_STATUS,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_MODEM_STATUS.MODEM_PEND = ezradioCmd[0];
		ezradioReply->GET_MODEM_STATUS.MODEM_STATUS = ezradioCmd[1];
		ezradioReply->GET_MODEM_STATUS.CURR_RSSI = ezradioCmd[2];
		ezradioReply->GET_MODEM_STATUS.LATCH_RSSI = ezradioCmd[3];
		ezradioReply->GET_MODEM_STATUS.ANT1_RSSI = ezradioCmd[4];
		ezradioReply->GET_MODEM_STATUS.ANT2_RSSI = ezradioCmd[5];
		ezradioReply->GET_MODEM_STATUS.AFC_FREQ_OFFSET = ((uint16_t)ezradioCmd[6] << 8) & 0xFF00;
		ezradioReply->GET_MODEM_STATUS.AFC_FREQ_OFFSET |= (uint16_t)ezradioCmd[7] & 0x00FF;
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Clear all Chip status flags. Does NOT read back interrupt flags
 *
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_chip_status_fast_clear(SI4455_HAL_typedef_t *hal)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[2u];
	ezradioCmd[0] = EZRADIO_CMD_ID_GET_CHIP_STATUS;
	ezradioCmd[1] = 0;

	if (SI4455_COMM_send_cmd(hal, 2u, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Clear and read all Chip status flags.
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_get_chip_status_fast_clear_read(SI4455_HAL_typedef_t *hal,
																	 ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_GET_CHIP_STATUS];

	ezradioCmd[0] = EZRADIO_CMD_ID_GET_CHIP_STATUS;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  1u,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_GET_CHIP_STATUS,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->GET_CHIP_STATUS.CHIP_PEND = ezradioCmd[0];
		ezradioReply->GET_CHIP_STATUS.CHIP_STATUS = ezradioCmd[1];
		ezradioReply->GET_CHIP_STATUS.CMD_ERR_STATUS = ezradioCmd[2];
	}

	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Resets the RX/TX FIFO. Does not read back anything from TX/RX FIFO
 *
 * @param[in] fifo FIFO to be reset.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_fifo_info_fast_reset(SI4455_HAL_typedef_t *hal, uint8_t fifo)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[2u];

	ezradioCmd[0] = EZRADIO_CMD_ID_FIFO_INFO;
	ezradioCmd[1] = fifo;

	if (SI4455_COMM_send_cmd(hal, 2u, ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}
	return EZRADIO_CONFIG_SUCCESS;
}

/**
 * Reads RX/TX FIFO count space. Does NOT reset RX/TX FIFO
 *
 * @param[out] ezradioReply   Reply structure of the command.
 */
SI4455_CONFIG_return_t SI4455_CONFIG_fifo_info_fast_read(SI4455_HAL_typedef_t *hal,
														 ezradio_cmd_reply_t *ezradioReply)
{
	/* EZRadio command buffer */
	uint8_t ezradioCmd[EZRADIO_CMD_REPLY_COUNT_FIFO_INFO];

	ezradioCmd[0] = EZRADIO_CMD_ID_FIFO_INFO;

	if (SI4455_COMM_send_cmd_get_resp(hal,
									  1u,
									  ezradioCmd,
									  EZRADIO_CMD_REPLY_COUNT_FIFO_INFO,
									  ezradioCmd) != SI4455_COMM_SUCCESS)
	{
		return EZRADIO_CONFIG_CTS_TIMEOUT;
	}

	if (ezradioReply != NULL)
	{
		ezradioReply->FIFO_INFO.RX_FIFO_COUNT = ezradioCmd[0];
		ezradioReply->FIFO_INFO.TX_FIFO_SPACE = ezradioCmd[1];
	}
	return EZRADIO_CONFIG_SUCCESS;
}
