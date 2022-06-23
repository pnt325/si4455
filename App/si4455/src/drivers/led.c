#include "led.h"
#include "pin_config.h"
#include "config.h"

bool _ledIsInit = false;

static void gpioSet(GPIO_Port_TypeDef port, unsigned int pin, uint8_t status);

void ledInit(void)
{
	debugLogInfo("LED Init");
	DISABLE_HW();
	GPIO_PinModeSet(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(GPIO_LED_YELLOW_PORT, GPIO_LED_YELLOW_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN, gpioModePushPull, 0);
	_ledIsInit = true;
}

bool ledTest(void)
{
	return _ledIsInit;
}

void ledSet(Led_t led, uint8_t status)
{
	DISABLE_HW();
	assert_true(led < LED_NUM);
	switch (led)
	{
	case LED_GREEN:
		gpioSet(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN, status);
		break;
	case LED_YELLOW:
		gpioSet(GPIO_LED_YELLOW_PORT, GPIO_LED_YELLOW_PIN, status);
		break;
	case LED_RED:
		gpioSet(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN, status);
		break;
	default:
		break;
	}
}

static void gpioSet(GPIO_Port_TypeDef port, unsigned int pin, uint8_t status)
{
	DISABLE_HW();
	if (status)
	{
		GPIO_PinOutSet(port, pin);
	}
	else
	{
		GPIO_PinOutClear(port, pin);
	}
}
