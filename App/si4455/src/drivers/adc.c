#include <stdio.h>
#include "adc.h"
#include "em_iadc.h"
#include "em_cmu.h"
#include "dmadrv.h"

#include "../configs/config.h"
#include "pin_config.h"

#define ADC_DMA_INVALID_CHANNEL 0xFF
#define CLK_SRC_ADC_FREQ        10000000
#define CLK_ADC_FREQ            10000000
#define ADC_RADAR_SCAN_CYCLE    1280   /** Hz */

static bool         adcInitFlags[ADC_NUM] = {false};
static unsigned int adcDmaScanChannel = ADC_DMA_INVALID_CHANNEL; 
static void         (*scanCompletedCallback)(void);

static bool adcDmaScanDone(unsigned int channel, unsigned int seq, void *userParam);
//static void adcBatteryInit(void);
//static void adcRadarInit(unsigned int scanFreq);

void adcInit(Adc_t adc)
{
	DISABLE_HW();
    assert_true(adc < ADC_NUM);
    
    if(adcInitFlags[adc])
    {
        return;
    }

    // Reset last init
    adcInitFlags[ADC_BATTERY] = false;
    adcInitFlags[ADC_RADAR] = false;

    if (adc == ADC_BATTERY)
    {
        IADC_Init_t init = IADC_INIT_DEFAULT;
        IADC_AllConfigs_t allConfigs = IADC_ALLCONFIGS_DEFAULT;
        IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
        IADC_SingleInput_t input = IADC_SINGLEINPUT_DEFAULT;

        CMU_ClockEnable(cmuClock_IADC0, true);
        allConfigs.configs[0].reference = iadcCfgReferenceVddx;
        /* Only configure the ADC if it is not already running */
        if (IADC0->CTRL == _IADC_CTRL_RESETVALUE)
        {
            IADC_init(IADC0, &init, &allConfigs);
        }

        GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0;
        input.posInput = GPIO_BAT_VOLT_ADC;
        input.negInput = iadcNegInputGnd;

        IADC_initSingle(IADC0, &initSingle, &input);
        IADC_enableInt(IADC0, IADC_IEN_SINGLEDONE);

        adcInitFlags[adc] = true;
    }
    else // Radar init
    {
        IADC_Init_t adcInit = IADC_INIT_DEFAULT;
        IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
        IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;
        IADC_ScanTable_t initScanTable = IADC_SCANTABLE_DEFAULT; // Scan Table

        GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN0_ADC0;
        GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AODD0_ADC0;

        CMU_ClockEnable(cmuClock_IADC0, true);
        IADC_reset(IADC0);

        CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

        adcInit.warmup         = iadcWarmupKeepWarm;
        adcInit.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);
        adcInit.timerCycles    = ADC_RADAR_SCAN_CYCLE;

        initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
        initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                           CLK_ADC_FREQ,
                                                                           0,
                                                                           iadcCfgModeNormal,
                                                                           adcInit.srcClkPrescale);

        initScan.dataValidLevel = _IADC_SCANFIFOCFG_DVL_VALID2;
        initScan.triggerAction  = iadcTriggerActionOnce;
        initScan.triggerSelect  = iadcTriggerSelTimer;
        initScan.fifoDmaWakeup  = true;

        initScanTable.entries[0].posInput      = GPIO_RADAR_I_ADC;
        initScanTable.entries[0].negInput      = iadcNegInputGnd;
        initScanTable.entries[0].includeInScan = true;
        initScanTable.entries[1].posInput      = GPIO_RADAR_Q_ADC;
        initScanTable.entries[1].negInput      = iadcNegInputGnd;
        initScanTable.entries[1].includeInScan = true;

        IADC_init(IADC0, &adcInit, &initAllConfigs);
        IADC_initScan(IADC0, &initScan, &initScanTable);
        IADC_command(IADC0, iadcCmdEnableTimer);
        adcInitFlags[adc] = true;
    }
}

bool adcTest(Adc_t adc)
{
    assert_true(adc < ADC_NUM);
    return adcInitFlags[adc];
}

void adcScan(Adc_t adc, uint16_t *buffer, uint32_t size, void (*callback)(void))
{
	DISABLE_HW();
    assert_true((adc < ADC_NUM) && buffer && size);
    if(adc == ADC_RADAR)
    {
        assert_true(callback);
    }

    if (adc == ADC_BATTERY)
    {
        IADC_clearInt(IADC0, IADC_IF_SINGLEDONE);
        IADC_command(IADC0, IADC_CMD_SINGLESTART);
        while (!(IADC_getInt(IADC0) & IADC_IF_SINGLEDONE))
        {
            ;
        }
        *buffer = IADC_readSingleData(IADC0);
    }
    else if(adc == ADC_RADAR)
    {
        assert_true(adcDmaScanChannel != ADC_DMA_INVALID_CHANNEL);
        scanCompletedCallback = callback;
        Ecode_t err = DMADRV_PeripheralMemory(adcDmaScanChannel,
                                              dmadrvPeripheralSignal_IADC0_IADC_SCAN,
                                              (void *)(buffer),
                                              (void *)&IADC0->SCANFIFODATA,
                                              1,
                                              size,
                                              dmadrvDataSize2,
                                              adcDmaScanDone,
                                              NULL);
        assert_true(err == ECODE_EMDRV_DMADRV_OK);
        IADC_command(IADC0, iadcCmdStartScan);
    }
    else
    {
        assert_true(false);
    }
}

void adcScanStop(void)
{
	DISABLE_HW();
    if(adcInitFlags[ADC_RADAR])
    {
        IADC_command(IADC0, iadcCmdStopScan);
    }
}

void adcReset(void)
{
	DISABLE_HW();
    if (adcInitFlags[ADC_BATTERY] || adcInitFlags[ADC_RADAR])
    {
        IADC_reset(IADC0);
        CMU_ClockEnable(cmuClock_IADC0, false); // disable iadc clock.
    }
    adcInitFlags[ADC_BATTERY] = false;
    adcInitFlags[ADC_RADAR] = false;
}

void adcDmaChannelSet(unsigned int channel)
{
    adcDmaScanChannel = channel;
}

static bool adcDmaScanDone(unsigned int channel, unsigned int seq, void *userParam)
{
    (void)userParam;
    (void)channel;
    (void)seq;

    if (scanCompletedCallback != NULL)
    {
        scanCompletedCallback();
    }
    return true;
}
