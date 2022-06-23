#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	LED_GREEN = 0, LED_YELLOW, LED_RED, LED_NUM
} Led_t;

void ledInit(void);
bool ledTest(void);
void ledSet(Led_t led, uint8_t status);

#define ledGreenSet(v)  ledSet(LED_GREEN, v)
#define ledYellowSet(v) ledSet(LED_YELLOW, v)
#define ledRedSet(v)    ledSet(LED_RED, v)

#endif /*_LED_H_*/
