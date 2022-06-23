#include "buzzer.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "pin_config.h"
#include "config.h"

#ifdef USE_LETIMER
#include "em_letimer.h"
#define LETIMER_OUT_FREQ 4 /** 4 kHz */
#else
#define BUZZER_TIMER_PWM_DEFAULT_FREQ 4000 // hz
#endif

static bool _buzzerIsInit = false;
static uint8_t _buzzerEnable = false;

void buzzerInit(void)
{
	debugLogInfo("Buzzer Init");
	DISABLE_HW();
#ifdef USE_LETIMER
	CMU_ClockEnable(cmuClock_LETIMER0, true);
	LETIMER_Init_TypeDef leTimerInit = LETIMER_INIT_DEFAULT;

	uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / LETIMER_OUT_FREQ;
	leTimerInit.comp0Top = true;
	leTimerInit.topValue = topValue;
	leTimerInit.ufoa0 = letimerUFOAPwm;
	leTimerInit.repMode = letimerRepeatFree;
	leTimerInit.enable = false;

	GPIO->LETIMERROUTE[0].ROUTEEN = GPIO_LETIMER_ROUTEEN_OUT0PEN;
	GPIO->LETIMERROUTE[0].OUT0ROUTE = (GPIO_BUZZER_PORT
									   << _GPIO_LETIMER_OUT0ROUTE_PORT_SHIFT) |
									  (GPIO_BUZZER_PIN << _GPIO_LETIMER_OUT0ROUTE_PIN_SHIFT);
	LETIMER_CompareSet(LETIMER0, 0, topValue / 2); /** 50% */
	LETIMER_Init(LETIMER0, &leTimerInit);

#else
	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
	TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

	timerInit.prescale = timerPrescale8;
	timerInit.enable = false;
	timerCCInit.mode = timerCCModePWM;

	GPIO_PinModeSet(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(GPIO_BUZZER_ENABLE_PORT, GPIO_BUZZER_ENABLE_PIN, gpioModePushPull, 0);
	CMU_ClockEnable(cmuClock_TIMER1, true);
	TIMER_Init(TIMER1, &timerInit);

	GPIO->TIMERROUTE[1].ROUTEEN = GPIO_TIMER_ROUTEEN_CC0PEN;
	GPIO->TIMERROUTE[1].CC0ROUTE = (GPIO_BUZZER_PORT << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT) | (GPIO_BUZZER_PIN << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

	TIMER_InitCC(TIMER1, 0, &timerCCInit);

	uint32_t timerFreq = CMU_ClockFreqGet(cmuClock_TIMER1) / (timerInit.prescale + 1);
	uint32_t topValue = (timerFreq / BUZZER_TIMER_PWM_DEFAULT_FREQ);
	TIMER_TopSet(TIMER1, topValue);
	TIMER_CompareSet(TIMER1, 0, (uint32_t)(topValue / 2));
	TIMER_Enable(TIMER1, false);
#endif
	_buzzerIsInit = true;
	_buzzerEnable = false;
}

bool buzzerTest(void)
{
	return _buzzerIsInit;
}

void buzzerSet(uint8_t status)
{
	DISABLE_HW();
	assert_true(_buzzerIsInit);
	if (status)
	{
		GPIO_PinOutSet(GPIO_BUZZER_ENABLE_PORT, GPIO_BUZZER_ENABLE_PIN);
#ifdef USE_LETIMER
		LETIMER_Enable(LETIMER0, true);
#else
		TIMER_Enable(TIMER1, true);
#endif
	}
	else
	{
		GPIO_PinOutClear(GPIO_BUZZER_ENABLE_PORT, GPIO_BUZZER_ENABLE_PIN);
		GPIO_PinOutClear(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);

#ifdef USE_LETIMER
		LETIMER_Enable(LETIMER0, false);
#else
		TIMER_Enable(TIMER1, false);
#endif
	}
}
