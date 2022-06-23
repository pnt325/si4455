/*
 * si4455_config.h
 *
 *  Created on: Mar 7, 2022
 *      Author: Spirit_Nguyen
 */

#ifndef SRC_MODULES_EZRADIO_SI4455_CONFIG_H_
#define SRC_MODULES_EZRADIO_SI4455_CONFIG_H_

#include "config.h"
#include <stdint.h>

#define si4455_assert(_v)			assert_true(_v)
#define si4455_log(_c, ...)			debugLog(_c, ##__VA_ARGS__)

#endif /* SRC_MODULES_EZRADIO_SI4455_CONFIG_H_ */
