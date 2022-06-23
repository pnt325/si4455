#include "button.h"
#include "pin_config.h"
#include "config.h"

void buttonInit(void)
{
    debugLogInfo("Button Init");
	DISABLE_HW();
    GPIO_PinModeSet(GPIO_USER_BUTTON_PORT, GPIO_USER_BUTTON_PIN, gpioModeInputPullFilter, 1);
    GPIO_IntConfig(GPIO_USER_BUTTON_PORT, GPIO_USER_BUTTON_PIN, true, true, true);
}

bool buttonPressed(void)
{
#ifdef DISABLE_ALL_HARDWARE
	return true;
#endif
    if (GPIO_PinInGet(GPIO_USER_BUTTON_PORT, GPIO_USER_BUTTON_PIN) == 0x00)
    {
        return true;
    }
    return false;
}

void buttonInitEM4(void)
{
	DISABLE_HW();
    GPIO_EM4EnablePinWakeup(0x08 << _GPIO_EM4WUEN_EM4WUEN_SHIFT, 0);
}
