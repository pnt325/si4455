#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	ADC_BATTERY = 0, ADC_RADAR, ADC_NUM
} Adc_t;

void adcInit(Adc_t adc);
bool adcTest(Adc_t adc);
void adcScan(Adc_t adc, uint16_t* buffer, uint32_t size, void (*callback)(void));
void adcScanStop(void);
void adcReset(void);
void adcDmaChannelSet(unsigned int channel);

#endif /*_ADC_H_*/
