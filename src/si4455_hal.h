#ifndef _SI4455_HAL_H_
#define _SI4455_HAL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SI4455_HAL_s{
	uint8_t _cts;														//! CTS signal
	void    (*delay_ms)				(uint32_t ms);						//! Delay ms
	void    (*assert_shutdown)		(void);								//! Shutdown SI4455
	void    (*deassert_shutdown)	(void);								//! Enable SI4455
	void    (*set_nsel)				(uint8_t status);					//! SI4455 SPI NSEL pin control
	uint8_t (*nirq_status)			(void);								//! SI4455 IRQ pin status
	void	(*spi_write_byte)		(uint8_t byte);						//! SI4455 SPI write a byte
	void	(*spi_read_byte)		(uint8_t* data);					//! SI4455 SPI read a byte
	void	(*spi_write_data)		(struct SI4455_HAL_s* hal ,uint8_t len, const uint8_t* data);	//! SI4455 SPI write array of data
	void	(*spi_read_data)		(struct SI4455_HAL_s* hal ,uint8_t len, uint8_t* data);		//! SI4455 SPI read array of data
}SI4455_HAL_typedef_t;

void SI4455_HAL_init(SI4455_HAL_typedef_t *hal,
					 void (*delay_ms)(uint32_t ms),							  
					 void (*assert_shutdown)(void),							  
					 void (*deassert_shutdown)(void),						  
					 void (*set_nsel)(uint8_t status),						  
					 uint8_t (*nirq_status)(void),							  
					 void (*spi_write_byte)(uint8_t byte),					  
					 void (*spi_read_byte)(uint8_t *data));

#ifdef __cplusplus
}
#endif

#endif /*_SI4455_HAL_H_*/
