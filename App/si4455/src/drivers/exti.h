#ifndef _EXINIT_H_
#define _EXINIT_H_

#include <stdint.h>

void extiInit(void);

void extiButtonCallback(uint8_t state);
void extiSi4455Callback(uint8_t state);

#endif /*_EXINIT_H_*/