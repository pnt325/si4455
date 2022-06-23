#include "exti.h"
#include "pin_config.h"
#include "config.h"

void extiInit(void)
{
    debugLogInfo("External Interrupt Init");
	DISABLE_HW();
    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

void __attribute__((weak)) extiButtonCallback(uint8_t state) 
{
    /** Do nothing */
}

void __attribute__((weak)) extiSi4455Callback(uint8_t state) 
{
    /** Do nothing */
}

static void button_irq(uint32_t interruptMask)
{
    uint8_t state;
    if (interruptMask & (1 << GPIO_SI4455_nIRQ_PIN))
    {
        extiSi4455Callback(0);
    }

    else if (interruptMask & (1 << GPIO_USER_BUTTON_PIN))
    {
        state = GPIO_PinInGet(GPIO_USER_BUTTON_PORT, GPIO_USER_BUTTON_PIN);
        extiButtonCallback(state);
    }
}

/**************************************************************************/ /**
 * @brief GPIO Interrupt handler for even pins.
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
	DISABLE_HW();
    /* Get and clear all pending GPIO interrupts */
    uint32_t interruptMask = GPIO_IntGet();
    GPIO_IntClear(interruptMask);
    button_irq(interruptMask);
}

/**************************************************************************/ /**
 * @brief GPIO Interrupt handler for even pins.
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	DISABLE_HW();
    /* Get and clear all pending GPIO interrupts */
    uint32_t interruptMask = GPIO_IntGet();
    GPIO_IntClear(interruptMask);
    button_irq(interruptMask);
}
