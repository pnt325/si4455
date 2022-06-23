#ifndef _SI4455_COMM_H_
#define _SI4455_COMM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "si4455_hal.h"

typedef enum
{
	SI4455_COMM_SUCCESS,
	SI4455_COMM_TIMEOUT
} SI4455_COMM_return_t;

SI4455_COMM_return_t SI4455_COMM_get_resp(SI4455_HAL_typedef_t *hal, uint8_t byteCount, uint8_t *pData);
SI4455_COMM_return_t SI4455_COMM_send_cmd(SI4455_HAL_typedef_t *hal, uint8_t byteCount, uint8_t *pData);
SI4455_COMM_return_t SI4455_COMM_read_data(SI4455_HAL_typedef_t *hal, uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t *pData);
SI4455_COMM_return_t SI4455_COMM_write_data(SI4455_HAL_typedef_t *hal, uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t *pData);
SI4455_COMM_return_t SI4455_COMM_poll_cts(SI4455_HAL_typedef_t *hal);
SI4455_COMM_return_t SI4455_COMM_send_cmd_get_resp(SI4455_HAL_typedef_t *hal, uint8_t cmdByteCount, uint8_t *pCmdData, uint8_t respByteCount, uint8_t *pRespData);
void 			     SI4455_COMM_clear_cts(SI4455_HAL_typedef_t *hal);

#ifdef __cplusplus
}
#endif

#endif /*_SI4455_COMM_H_*/
