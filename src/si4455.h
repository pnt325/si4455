
#ifndef _SI4455_H_
#define _SI4455_H_

#include <stdbool.h>
#include "si4455_cmd.h"
#include "si4455_hal.h"

#define SI4455_PACKET_SIZE	64

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct  {
	uint8_t 				rx_buf[SI4455_PACKET_SIZE];	//! Receive data buffer
	uint8_t 				channel;					//! Transmit receive channel
	uint8_t 				is_init;					//! SI4455 initialize status.
	SI4455_HAL_typedef_t 	hal;						//! hal object
	void					(*rx_cplt)(uint8_t* data);	//! Rx callback function
	void					(*tx_cplt)(void);			//! Tx callback function
	void					(*crc_err)(void);			//! Crc error callback function
} SI4455_t;

bool SI4455_init(SI4455_t *si4455,
				 void (*rx_cplt)(uint8_t *data),
				 void (*tx_cplt)(void),
				 void (*crc_err)(void),
				 uint8_t channel,
				 SI4455_HAL_typedef_t *hal);
bool SI4455_start_tx(SI4455_t* si4455, uint8_t len, uint8_t *data);
bool SI4455_start_rx(SI4455_t* si4455);
bool SI4455_handle(SI4455_t* si4455);
bool SI4455_reset_txrx_fifo(SI4455_t* si4455);
bool SI4455_set_standby(SI4455_t* si4455);
bool SI4455_shutdown(SI4455_t* si4455);
bool SI4455_set_syncword(SI4455_t* si4455, uint8_t* syncWord);
bool SI4455_get_syncword(SI4455_t* si4455, uint8_t* syncword);
bool SI4455_set_channel(SI4455_t* si4455, uint8_t channel);
uint8_t SI4455_get_channel(SI4455_t* si4455);

#ifdef __cplusplus
}
#endif

#endif /* _SI4455_H_ */
