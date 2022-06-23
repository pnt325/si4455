#ifndef _SI4455_API_LIB_ADD_H_
#define _SI4455_API_LIB_ADD_H_

#include "si4455_api_lib.h"
#include "si4455_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

SI4455_CONFIG_return_t SI4455_CONFIG_init(SI4455_HAL_typedef_t *hal, const uint8_t *pSetPropCmd);
SI4455_CONFIG_return_t SI4455_CONFIG_write_ezconfig_array(SI4455_HAL_typedef_t *hal, uint8_t numBytes, uint8_t *pEzConfigArray);
SI4455_CONFIG_return_t SI4455_CONFIG_check_ezconfig(SI4455_HAL_typedef_t *hal, uint16_t checksum, ezradio_cmd_reply_t *ezradioReply);

#ifdef __cplusplus
}
#endif

#endif /*_SI4455_API_LIB_ADD_H_*/
