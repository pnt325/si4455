#include "config.h"
#include "pwr_boot.h"
#include "pin_config.h"
#include "em_cmu.h"

void pwrBootInit(void)
{
	DISABLE_HW();
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(GPIO_BOOT_ENABLE_PORT, GPIO_BOOT_ENABLE_PIN, gpioModePushPull, 1);
}

void pwrBootEnable(bool en)
{
	DISABLE_HW();
    if (en)
    {
        GPIO_PinOutSet(GPIO_BOOT_ENABLE_PORT, GPIO_BOOT_ENABLE_PIN);
    }
    else
    {
        GPIO_PinOutClear(GPIO_BOOT_ENABLE_PORT, GPIO_BOOT_ENABLE_PIN);
    }
}
