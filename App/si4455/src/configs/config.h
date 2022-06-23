#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

/** Version 
 * Add new BLE protocol handle
*/
#define VERSION_MAJOR   2       // 0-9
#define VERSION_MINOR   2       // 0-9
#define VERSION_PATCH   0       // 0-9
#define VERSION_BUILD	7       // 0-15


#define getVersion()  	(uint16_t)(VERSION_MAJOR | (VERSION_MINOR << 4) | (VERSION_PATCH << 8) | (VERSION_BUILD << 12))
// void ver_deserialize(uint16_t version)
// {
// 	printf("Version deserialize: %d.%d.%d.%d\r\n", 
//      (version & 0x000f),
// 		((version >> 4) & 0x000f),
// 		((version >> 8) & 0x000f),
// 		((version >> 12) & 0x000f));
// 	printf("Version deserialize\r\nMajor:%d\r\nMinor:%d\r\nPatch:%d\r\nBuild:%d\r\n", 
//      (version & 0x000f),
// 		((version >> 4) & 0x000f),
// 		((version >> 8) & 0x000f),
// 		((version >> 12) & 0x000f));
// }

#define get_max(a,b)		(a>b?a:b)

#define APP_FAILURE_CHECK_MAX           5			/** Times */
#define APP_RESTART_PERIOD              10          /** ms */
#define APP_IDLE_PERIOD                 30000
#define APP_OVEN_PERIOD                 10000       // ms
#define APP_SAFETY_PERIOD               10000       // ms   
#define APP_FAILURE_PERIOD              10000       // ms
#define APP_BLE_WDOG_TIMEOUT            30000       /** ms */

#define APP_MCU_WORKING_TEMPERATURE_LIMIT   80          // degree

#define APP_TEST_FUNC_PERIOD            3000        // ms

// SI4455 revision configure
#define SI4455_REV_B    0
#define SI4455_REV_C    1
#define SI4455_REV      SI4455_REV_C

// Button
#define USER_BUTTON_INIT_ON_BOOT

// #define DISABLE_ALL_HARDWARE
#ifdef DISABLE_ALL_HARDWARE
#define DISABLE_HW()		return;
#else
#define  DISABLE_HW()
#endif

#define DEBUG_ASSERT
#define DEBUG_LOG
#define DEBUG_TRACE
#define UNUSE(v)	(void)

// watchdog config macro
// #ifndef  DEBUG_LOG
#define USE_WDOG
// #endif

#ifdef DEBUG_LOG
#include "SEGGER_RTT.h"
#define debugLogInit()           SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP)
#define debugLog(c, ...)         SEGGER_RTT_printf(0, c,##__VA_ARGS__)
#define debugLogln(c, ...)       SEGGER_RTT_printf(0, c"\r\n",##__VA_ARGS__)
#define debugLogInfo(c, ...)     SEGGER_RTT_printf(0, "[INFO]"c"\r\n",##__VA_ARGS__)
#define debugLogError(c, ...)    SEGGER_RTT_printf(0, "[ERROR]"c"\r\n",##__VA_ARGS__)
#else
#define debugLogInit()
#define debugLog(c, ...) 
#define debugLogLn(c, ...)
#define debugLogInfo(c, ...)
#define debugLogError(c, ...)
#endif

extern void elpro_FaultHandle(void);

#ifdef DEBUG_ASSERT
extern void failureHandle(const char* file, int line);
#define assert_true(v)          (v)?(void)0: failureHandle(__FILE__, __LINE__)
#else 
#define assert_true(v)          (v)?(void)0: elpro_FaultHandle();
#endif 

#ifdef DEBUG_TRACE
#else
#endif

#endif /*_CONFIG_H_*/
