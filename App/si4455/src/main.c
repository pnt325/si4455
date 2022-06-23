#include "mcu.h"
#include "config.h"

#include "si4455_test.h"

int main(void)
{
	mcuInit();
	SI4455_TEST_init();

	while (1)
	{
		SI4455_TEST_run();
	}
	return 0;
}

void _fault_handle(void)
{
	__disable_irq();	// disable all interrupt
}

#ifdef DEBUG_ASSERT
/**
 * @brief Failure handle. direct call from assert_true.
 */
void failureHandle(const char* file, int line)
{
	debugLogError("%s, %d", file, line);
	_fault_handle();
	while(1)
	{
		__NOP();
	}
}
#endif 

void elpro_FaultHandle(void)
{
	/**
	 * Red LED on and buzzer beep is system faultHandle
	 */
	_fault_handle();
	while(1)
	{
		__NOP();
	}
}
