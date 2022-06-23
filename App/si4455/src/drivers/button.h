#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <stdint.h>
#include <stdbool.h>

enum {
    BUTTON_PRESSED = 0,
    BUTTON_RELEASED = 1
};

void buttonInit(void);
bool buttonPressed(void);
void buttonInitEM4(void);

#endif /*_BUTTON_H_*/
