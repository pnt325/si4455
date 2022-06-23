#include "si4455_hal.h"
#include "si4455_config.h"

static void hal_spi_read_data(SI4455_HAL_typedef_t *hal, uint8_t len, uint8_t* data);
static void hal_spi_write_data(SI4455_HAL_typedef_t *hal, uint8_t len, const uint8_t *data);

void SI4455_HAL_init(SI4455_HAL_typedef_t *hal,
                     void (*delay_ms)(uint32_t ms),
                     void (*assert_shutdown)(void),
                     void (*deassert_shutdown)(void),
                     void (*set_nsel)(uint8_t status),
                     uint8_t (*nirq_status)(void),
                     void (*spi_write_byte)(uint8_t byte),
                     void (*spi_read_byte)(uint8_t *data))
{
    si4455_assert(hal               && delay_ms      && assert_shutdown &&
                  deassert_shutdown && set_nsel      && nirq_status     && 
                  spi_write_byte    && spi_read_byte);
    
    hal->_cts              = 0;
    hal->delay_ms          = delay_ms;
    hal->assert_shutdown   = assert_shutdown;
    hal->deassert_shutdown = deassert_shutdown;
    hal->set_nsel          = set_nsel;
    hal->nirq_status       = nirq_status;
    hal->spi_write_byte    = spi_write_byte;
    hal->spi_read_byte     = spi_read_byte;
    hal->spi_read_data     = hal_spi_read_data;
    hal->spi_write_data    = hal_spi_write_data;
}

static void hal_spi_read_data(SI4455_HAL_typedef_t* hal, uint8_t len, uint8_t* data)
{
	si4455_assert(data && len && hal);
	while(len--)
	{
		hal->spi_read_byte(data);
		data++;
	}
}

static void hal_spi_write_data(SI4455_HAL_typedef_t *hal, uint8_t len, const uint8_t *data)
{
	si4455_assert(hal && len && data);
	while(len--)
	{
		hal->spi_write_byte(*data);
		data++;
	}
}
