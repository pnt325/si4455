#ifndef _SI4455_API_LIB_H_
#define _SI4455_API_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "si4455_cmd.h"
#include "SI4455_hal.h"

typedef enum
{
	EZRADIO_CONFIG_SUCCESS,		 /**< Configuration succeded. */
	EZRADIO_CONFIG_NO_PATCH,	 /**< No patch is given. */
	EZRADIO_CONFIG_CTS_TIMEOUT,	 /**< CTS timeout error during configuration. */
	EZRADIO_CONFIG_PATCH_FAIL,	 /**< Patching is failed. */
	EZRADIO_CONFIG_COMMAND_ERROR /**< Command error during configuration. */
} SI4455_CONFIG_return_t;

/* Minimal driver support functions */
SI4455_CONFIG_return_t SI4455_CONFIG_reset(SI4455_HAL_typedef_t *hal);
SI4455_CONFIG_return_t SI4455_CONFIG_power_up(SI4455_HAL_typedef_t *hal, uint8_t boot_options, uint8_t xtal_options, uint32_t xo_freq);
SI4455_CONFIG_return_t SI4455_CONFIG_part_info(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_start_tx(SI4455_HAL_typedef_t *hal, uint8_t channel, uint8_t condition, uint16_t tx_len);
SI4455_CONFIG_return_t SI4455_CONFIG_start_rx(SI4455_HAL_typedef_t *hal, uint8_t channel, uint8_t condition, uint16_t rx_len, uint8_t next_state1, uint8_t next_state2, uint8_t next_state3);
SI4455_CONFIG_return_t SI4455_CONFIG_get_int_status(SI4455_HAL_typedef_t *hal, uint8_t ph_clr_pend, uint8_t modem_clr_pend, uint8_t chip_clr_pend, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_gpio_pin_cfg(SI4455_HAL_typedef_t *hal, uint8_t gpio0, uint8_t gpio1, uint8_t gpio2, uint8_t gpio3, uint8_t nirq, uint8_t sdo, uint8_t gen_config, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_set_property(SI4455_HAL_typedef_t *hal, uint8_t group, uint8_t num_props, uint8_t start_prop, uint8_t *datas);
SI4455_CONFIG_return_t SI4455_CONFIG_change_state(SI4455_HAL_typedef_t *hal, uint8_t next_state1);

/* Extended driver support functions */
SI4455_CONFIG_return_t SI4455_CONFIG_nop(SI4455_HAL_typedef_t *hal);
SI4455_CONFIG_return_t SI4455_CONFIG_fifo_info(SI4455_HAL_typedef_t *hal, uint8_t fifo, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_write_tx_fifo(SI4455_HAL_typedef_t *hal, uint8_t numbytes, uint8_t *pdata);
SI4455_CONFIG_return_t SI4455_CONFIG_read_rx_fifo(SI4455_HAL_typedef_t *hal, uint8_t numbytes, uint8_t *prxdata);
SI4455_CONFIG_return_t SI4455_CONFIG_get_property(SI4455_HAL_typedef_t *hal, uint8_t group, uint8_t num_props, uint8_t start_prop, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_func_info(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_frr_a_read(SI4455_HAL_typedef_t *hal, uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_frr_b_read(SI4455_HAL_typedef_t *hal, uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_frr_c_read(SI4455_HAL_typedef_t *hal, uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_frr_d_read(SI4455_HAL_typedef_t *hal, uint8_t respByteCount, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_request_device_state(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_read_cmd_buff(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_get_ph_status(SI4455_HAL_typedef_t *hal, uint8_t ph_clr_pend, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_get_modem_status(SI4455_HAL_typedef_t *hal, uint8_t modem_clr_pend, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_get_chip_status(SI4455_HAL_typedef_t *hal, uint8_t chip_clr_pend, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_get_packet_info(SI4455_HAL_typedef_t *hal, uint8_t field_number_mask, uint16_t len, int16_t diff_len, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_start_tx_fast(SI4455_HAL_typedef_t *hal);
SI4455_CONFIG_return_t SI4455_CONFIG_start_rx_fast(SI4455_HAL_typedef_t *hal);
SI4455_CONFIG_return_t SI4455_CONFIG_get_int_status_fast_clear(SI4455_HAL_typedef_t *hal);
SI4455_CONFIG_return_t SI4455_CONFIG_get_int_status_fast_clear_read(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_gpio_pin_cfg_fast(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_get_ph_status_fast_clear(SI4455_HAL_typedef_t *hal);
SI4455_CONFIG_return_t SI4455_CONFIG_get_ph_status_fast_clear_read(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_get_modem_status_fast_clear(SI4455_HAL_typedef_t *hal);
SI4455_CONFIG_return_t SI4455_CONFIG_get_modem_status_fast_clear_read(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_get_chip_status_fast_clear(SI4455_HAL_typedef_t *hal);
SI4455_CONFIG_return_t SI4455_CONFIG_get_chip_status_fast_clear_read(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);
SI4455_CONFIG_return_t SI4455_CONFIG_fifo_info_fast_reset(SI4455_HAL_typedef_t *hal, uint8_t fifo);
SI4455_CONFIG_return_t SI4455_CONFIG_fifo_info_fast_read(SI4455_HAL_typedef_t *hal, ezradio_cmd_reply_t *ezradioReply);

#ifdef __cplusplus
}
#endif

#endif /*_SI4455_API_LIB_H_*/
