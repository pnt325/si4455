#include <stdbool.h>
#include "mcu.h"
#include "em_cmu.h"
#include "em_rtcc.h"
#include "hal-config-board.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "pwr_boot.h"
#include "pin_config.h"

#define CLK_LFXO_CTUNE (79U)
#define CLK_HFXO_FREQ (38400000UL)
#define DEVINFO_MODULEINFO_HFXOCALVAL_MASK 0x00080000UL
#define DEVINFO_MODULEINFO_CRYSTALOSCCALVAL (*((uint16_t *)(uint32_t)(DEVINFO_BASE + 0x8UL)))
#define DEVINFO_HFXOCTUNE_MASK 0x01FFUL

#define set_HFXO_CTUNE(val)          \
    do                               \
    {                                \
        hfxoInit.ctuneXoAna = (val); \
        hfxoInit.ctuneXiAna = (val); \
    } while (0)

void mcuInit(void)
{
    CHIP_Init();
    EMU_UnlatchPinRetention();
    CMU_ClockEnable(cmuClock_GPIO, true);

    /* DC-DC converter */
    EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
    dcdcInit.cmpThreshold = emuVreginCmpThreshold_2v1;
    EMU_DCDCInit(&dcdcInit);

    /* Clock initialize */
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
    if (0 == (DEVINFO->MODULEINFO & DEVINFO_MODULEINFO_HFXOCALVAL_MASK))
    {
        set_HFXO_CTUNE(DEVINFO->MODXOCAL & _DEVINFO_MODXOCAL_HFXOCTUNEXIANA_MASK);
    }
    else
    {
        set_HFXO_CTUNE(CLK_LFXO_CTUNE);
    }

    CMU_HFXOInit(&hfxoInit);
    SystemHFXOClockSet(CLK_HFXO_FREQ);

    CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);

    CMU_ClockSelectSet(cmuClock_RTCC, cmuSelect_LFRCO);
    CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFRCO);
    CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFRCO);
    CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_LFRCO);
    CMU_LFRCOSetPrecision(cmuPrecisionHigh);
    CMU_ClockEnable(cmuClock_RTCC, true);

    RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
    rtccInit.enable = true;
    rtccInit.debugRun = false;
    rtccInit.precntWrapOnCCV0 = false;
    rtccInit.cntWrapOnCCV1 = false;
    rtccInit.prescMode = rtccCntTickPresc;
    rtccInit.presc = rtccCntPresc_1;
    RTCC_Init(&rtccInit);

    /* EMU Initialize */
    EMU_EM01Init_TypeDef em01Init = EMU_EM01INIT_DEFAULT;
    EMU_EM23Init_TypeDef em23init = EMU_EM23INIT_DEFAULT;
    em23init.vScaleEM23Voltage = emuVScaleEM23_LowPower;

    EMU_EM01Init(&em01Init);
    EMU_EM23Init(&em23init);

    EMU->CTRL |= EMU_CTRL_EM2DBGEN;
}

float mcuGetTemperature(void)
{
    return EMU_TemperatureGet();
}
