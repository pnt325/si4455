#ifndef _BUZZER_H_
#define _BUZZER_H_

#include <stdint.h>
#include <stdbool.h>

// #define USE_LETIMER

void buzzerInit(void);
bool buzzerTest(void);
void buzzerSet(uint8_t status);
#define  buzzerOn()             buzzerSet(1)
#define  buzzerOff()            buzzerSet(0)

#endif /*_BUZZER_H_*/