#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "si4455_cmd.h"
#include "si4455_prop.h"
#include "si4455_hal.h"
#include "si4455_api_lib.h"
#include "si4455_api_lib_add.h"
#include "si4455.h"
#include "config.h"
#include "si4455_config.h"

#include "si4455_config_gen.h"

const uint8_t si4455_config_array[] = RADIO_CONFIGURATION_DATA_ARRAY;

static void si4455_power_up(SI4455_t* si4455);

bool SI4455_init(SI4455_t *si4455,
				 void (*rx_cplt)(uint8_t *data),
				 void (*tx_cplt)(void),
				 void (*crc_err)(void),
				 uint8_t channel,
				 SI4455_HAL_typedef_t* hal)
{
	uint8_t count = 5;	//! Retry times.
	si4455_assert(si4455 && rx_cplt && tx_cplt && crc_err && hal);

	si4455->rx_cplt = rx_cplt;
	si4455->tx_cplt = tx_cplt;
	si4455->crc_err = crc_err;
	si4455->channel = channel;
	si4455->hal     = *hal;

	si4455_power_up(si4455);
	while (count)
	{
		if (SI4455_CONFIG_init(&si4455->hal, si4455_config_array) == EZRADIO_CONFIG_SUCCESS)
			break;
		
		si4455_power_up(si4455);
		count--;
	}

	if (count == 0)
		return false;

	if (SI4455_CONFIG_get_int_status(&si4455->hal, 0u, 0u, 0u, NULL) != EZRADIO_CONFIG_SUCCESS)
		return false;

	si4455->is_init = 1;
	return true;
}

bool SI4455_handle(SI4455_t *si4455)
{
	/* EZRadio response structure union */
	ezradio_cmd_reply_t radioReplyData;
	si4455_assert(si4455 != NULL && si4455->is_init);

	/* Check is radio interrupt event. */
	if (si4455->hal.nirq_status() == 0x00)
	{
		/* Read ITs, clear all pending ones */
		if (SI4455_CONFIG_get_int_status(&si4455->hal, 0x00, 0x00, 0x00, &radioReplyData) != EZRADIO_CONFIG_SUCCESS)
		{
			return false;
		}

		//! Transmitted
		if (radioReplyData.GET_INT_STATUS.PH_PEND & EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_SENT_PEND_BIT)
		{
			if (si4455->tx_cplt)
			{
				si4455->tx_cplt();
			}
			return true;
		}

		//! Received
		if (radioReplyData.GET_INT_STATUS.PH_PEND & EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_PACKET_RX_PEND_BIT)
		{
			/* Check how many bytes we received. */
			if (SI4455_CONFIG_fifo_info(&si4455->hal, 0u, &radioReplyData) != EZRADIO_CONFIG_SUCCESS)
			{
				return false;
			}

			/* Read out the RX FIFO content. */
			if (SI4455_CONFIG_read_rx_fifo(&si4455->hal, radioReplyData.FIFO_INFO.RX_FIFO_COUNT, si4455->rx_buf) != EZRADIO_CONFIG_SUCCESS)
			{
				return false;
			}

			/* Note: Workaround for some FIFO issue. */
			if (SI4455_CONFIG_fifo_info(&si4455->hal, EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT, NULL) != EZRADIO_CONFIG_SUCCESS)
			{
				return false;
			}

			if (si4455->rx_cplt)
			{
				si4455->rx_cplt(si4455->rx_buf);
			}

			return true;
		}

		//! CRC error
		if (radioReplyData.GET_INT_STATUS.PH_PEND & EZRADIO_CMD_GET_INT_STATUS_REP_PH_PEND_CRC_ERROR_PEND_BIT)
		{
			/* Reset FIFO */
			if (SI4455_CONFIG_change_state(&si4455->hal, EZRADIO_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_READY) != EZRADIO_CONFIG_SUCCESS)
			{
				return false;
			}

			if (SI4455_CONFIG_fifo_info(&si4455->hal, EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT, NULL) != EZRADIO_CONFIG_SUCCESS)
			{
				return false;
			}

			if (si4455->crc_err)
			{
				si4455->crc_err();
			}

			return true;
		}

		//! Clear FIFO info
		if (SI4455_CONFIG_fifo_info(&si4455->hal, EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT, NULL) != EZRADIO_CONFIG_SUCCESS)
		{
			return false;
		}
	}

	return true;
}

bool SI4455_reset_txrx_fifo(SI4455_t *si4455)
{
	si4455_assert(si4455 != NULL && si4455->is_init);
	if (SI4455_CONFIG_fifo_info(&si4455->hal, EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT, NULL) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}

	if (SI4455_CONFIG_fifo_info(&si4455->hal, EZRADIO_CMD_FIFO_INFO_ARG_FIFO_TX_BIT, NULL) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}
	return true;
}

bool SI4455_set_standby(SI4455_t *si4455)
{
	si4455_assert(si4455 != NULL && si4455->is_init);
	if (SI4455_CONFIG_change_state(&si4455->hal, EZRADIO_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_SLEEP) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}

	return true;
}

bool SI4455_shutdown(SI4455_t *si4455)
{
	si4455_assert(si4455 != NULL && si4455->is_init);
	si4455->hal.assert_shutdown();
	si4455->is_init = 0;
	return true;
}

bool SI4455_set_syncword(SI4455_t *si4455, uint8_t *syncWord)
{
	ezradio_cmd_reply_t rep;
	bool ret = false;
	si4455_assert(si4455 != NULL && syncWord != NULL && si4455->is_init);
	if (syncWord)
	{
		rep.RAW[0] = 0x03;
		rep.RAW[1] = syncWord[0];
		rep.RAW[2] = syncWord[1];
		rep.RAW[3] = syncWord[2];
		rep.RAW[4] = syncWord[3];
		if (SI4455_CONFIG_set_property(&si4455->hal, 0x11, 0x05, 0, rep.RAW) != EZRADIO_CONFIG_SUCCESS)
		{
			return false;
		}

		if (SI4455_CONFIG_get_property(&si4455->hal, 0x11, 0x05, 0, &rep) != EZRADIO_CONFIG_SUCCESS)
		{
			return false;
		}
		if (rep.RAW[0] == 0x03 &&
			rep.RAW[1] == syncWord[0] &&
			rep.RAW[2] == syncWord[1] &&
			rep.RAW[3] == syncWord[2] &&
			rep.RAW[4] == syncWord[3])
		{
			ret = true;
		}
	}
	return ret;
}

bool SI4455_get_syncword(SI4455_t *si4455, uint8_t *syncword)
{
	ezradio_cmd_reply_t rep;
	si4455_assert(si4455 != NULL && syncword != NULL && si4455->is_init);
	if (syncword == NULL)
	{
		return false;
	}

	if (SI4455_CONFIG_get_property(&si4455->hal, 0x11, 0x05, 0, &rep) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}

	if (rep.RAW[0] != 0x03)
	{
		return false;
	}

	syncword[0] = rep.RAW[1];
	syncword[1] = rep.RAW[2];
	syncword[2] = rep.RAW[3];
	syncword[3] = rep.RAW[4];

	return true;
}

bool SI4455_set_channel(SI4455_t *si4455, uint8_t channel)
{
	si4455_assert(si4455 != NULL);
	si4455->channel = channel;
	return true;
}

uint8_t SI4455_get_channel(SI4455_t *si4455)
{
	si4455_assert(si4455 != NULL);
	return si4455->channel;
}

bool SI4455_start_tx(SI4455_t *si4455, uint8_t len, uint8_t *data)
{
	ezradio_cmd_reply_t ezradioReply;
	si4455_assert(si4455 != NULL && len > 0 && data != NULL && si4455->is_init);

	/* Request and check radio device state */
	if (SI4455_CONFIG_request_device_state(&si4455->hal, &ezradioReply) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}

	if (ezradioReply.REQUEST_DEVICE_STATE.CURR_STATE == EZRADIO_CMD_REQUEST_DEVICE_STATE_REP_CURR_STATE_MAIN_STATE_ENUM_TX)
	{
		return false;
	}

	/* Clear fifo before send */
	if (SI4455_CONFIG_fifo_info(&si4455->hal, EZRADIO_CMD_FIFO_INFO_ARG_FIFO_TX_BIT, NULL) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}

	/* Fill the TX fifo with datas */
	if (SI4455_CONFIG_write_tx_fifo(&si4455->hal, len, data) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}

	/* Start sending packet, channel 0, START immediately, Packet n bytes long, go READY when done */
	if (SI4455_CONFIG_start_tx(&si4455->hal, si4455->channel, 0x30, 0u) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}

	return true;
}

bool SI4455_start_rx(SI4455_t *si4455)
{
	si4455_assert(si4455 != NULL && si4455->is_init);
	/* Start Receiving packet, channel 0, START immediately, Packet n bytes long */
	if (SI4455_CONFIG_start_rx(&si4455->hal, si4455->channel, 0u, 0,
							   EZRADIO_CMD_START_RX_ARG_NEXT_STATE1_RXTIMEOUT_STATE_ENUM_NOCHANGE,
							   EZRADIO_CMD_START_RX_ARG_NEXT_STATE2_RXVALID_STATE_ENUM_READY,
							   EZRADIO_CMD_START_RX_ARG_NEXT_STATE3_RXINVALID_STATE_ENUM_RX) != EZRADIO_CONFIG_SUCCESS)
	{
		return false;
	}

	return true;
}

static void si4455_power_up(SI4455_t *si4455)
{
	/* Hardware reset the chip */
	SI4455_CONFIG_reset(&si4455->hal);
	si4455->hal.delay_ms(10);
}
