#include "spi.h"
#include "em_usart.h"
#include "em_cmu.h"

#include "pin_config.h"

#include "../configs/config.h"

#define SPI_PORT USART0
#define SPI_PORT_INDEX 0
#define SPI_SPEED 5000000 // hz
#define SPI_CLOCK_SOURCE cmuClock_USART0

bool _spiIsInit = false;

static void _spi_init(uint32_t speed_hz);

void spiInit(uint32_t speed_hz)
{
	DISABLE_HW();
    if(_spiIsInit)
    {
        return;
    }

    CMU_ClockEnable(SPI_CLOCK_SOURCE, true);

    /* GPIO init */
    GPIO_PinModeSet(GPIO_SPI0_MISO_PORT, GPIO_SPI0_MISO_PIN, gpioModeInput, 0);
    GPIO_PinModeSet(GPIO_SPI0_MOSI_PORT, GPIO_SPI0_MOSI_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(GPIO_SPI0_CLK_PORT, GPIO_SPI0_CLK_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(GPIO_SPI0_CS_PORT, GPIO_SPI0_CS_PIN, gpioModePushPull, 1);

    
    // GPIO routing
    GPIO->USARTROUTE[SPI_PORT_INDEX].RXROUTE = (GPIO_SPI0_MISO_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT) |
                                               (GPIO_SPI0_MISO_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
    GPIO->USARTROUTE[SPI_PORT_INDEX].TXROUTE = (GPIO_SPI0_MOSI_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT) |
                                               (GPIO_SPI0_MOSI_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
    GPIO->USARTROUTE[SPI_PORT_INDEX].CLKROUTE = (GPIO_SPI0_CLK_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT) |
                                                (GPIO_SPI0_CLK_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);
    GPIO->USARTROUTE[SPI_PORT_INDEX].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN |
                                               GPIO_USART_ROUTEEN_TXPEN |
                                               GPIO_USART_ROUTEEN_CLKPEN;

    // SPI Peripheral init
    _spi_init(speed_hz);
    _spiIsInit = true;
}

bool spiTest(void)
{
    return _spiIsInit;
}

void spiInitSpeed(uint32_t speed_hz)
{
    _spi_init(speed_hz);
}

uint8_t spiTransfer(uint8_t value)
{
#ifdef DISABLE_ALL_HARDWARE
	return 0;
#endif
    return USART_SpiTransfer(SPI_PORT, value);
}


// PRIVATE FUNCTION

static void _spi_init(uint32_t speed_hz)
{
    USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
    init.msbf = true;
    init.baudrate = speed_hz;

    debugLogInfo("SPI init speed: %u", speed_hz);
    USART_Reset(SPI_PORT);
    USART_InitSync(SPI_PORT, &init);
}
