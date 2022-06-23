/*
 * si4455_test.c
 *
 *  Created on: Mar 4, 2022
 *      Author: Spirit_Nguyen
 */

#include <string.h>
#include <stdio.h>
#include "si4455_test.h"
#include "spi.h"
#include "pin_config.h"
#include "config.h"
#include "ustimer.h"

//#define USE_RX

static SI4455_t si4455;

#ifdef USE_RX
static bool is_rx = false;
#else
static bool is_tx = false;
#endif
static void delay_ms(uint32_t ms)
{
	USTIMER_Init();
	/* Delay for 10ms time */
	while(ms)
	{
		USTIMER_Delay(1000u);
		ms--;
	}
	/* Deinit ustimer */
	USTIMER_DeInit();
}

static void si4455_assert_shutdown(void)
{
	GPIO_PinOutSet(GPIO_SI4455_SDN_PORT, GPIO_SI4455_SDN_PIN);
}

static void si4455_deassert_shutdown(void)
{
	GPIO_PinOutClear(GPIO_SI4455_SDN_PORT, GPIO_SI4455_SDN_PIN);
}

static void si4455_set_nsel(uint8_t status)
{
	if(status){
		GPIO_PinOutSet(GPIO_SPI0_CS_PORT, GPIO_SPI0_CS_PIN);
	}
	else {
		GPIO_PinOutClear(GPIO_SPI0_CS_PORT, GPIO_SPI0_CS_PIN);
	}
}

static uint8_t si4455_nirq_status(void)
{
	return GPIO_PinInGet(GPIO_SI4455_nIRQ_PORT, GPIO_SI4455_nIRQ_PIN);
}

static void spi_write_byte(uint8_t byte)
{
	spiTransfer(byte);
}

static void spi_read_byte(uint8_t* byte)
{
	*byte = spiTransfer(0xff);
}

static void tx_cplt(void)
{
	static uint32_t count = 0;
	debugLogInfo("TX callback: %d", count++);
#ifndef USE_RX
	is_tx = false;
#endif
}

static void rx_cplt(uint8_t* data)
{
	static uint32_t count = 0;
	debugLogInfo("RX callback: %d, Msg: %s", count++, data);
#ifdef USE_RX
	is_rx = true;
#endif
}

static void crc_err(void) {
	debugLogInfo("CRC_ERR callback");
}

#ifndef USE_RX
static void si4455_send(const char* msg)
{
	static uint32_t msg_count = 0;
	if(is_tx) {
		return ;
	}

	char send_buf[SI4455_PACKET_SIZE] = {0};
	sprintf(send_buf, "Send msg %d", (int)msg_count);
	msg_count++;
	if(SI4455_start_tx(&si4455, (uint8_t)strlen(send_buf), (uint8_t*)send_buf) == false) {
		debugLogError("Transmit Error");
		assert_true(false);
	}

	is_tx = true;
}
#endif


void SI4455_TEST_init(void)
{
	debugLogInit();

	spiInit(1000000);

	GPIO_PinModeSet(GPIO_SI4455_nIRQ_PORT, GPIO_SI4455_nIRQ_PIN, gpioModeInputPull, 1);
	GPIO_PinModeSet(GPIO_SI4455_SDN_PORT, GPIO_SI4455_SDN_PIN, gpioModePushPull, 1);

	SI4455_HAL_typedef_t si4455_hal;
	SI4455_HAL_init(&si4455_hal,
					delay_ms,
					si4455_assert_shutdown,
					si4455_deassert_shutdown,
					si4455_set_nsel,
					si4455_nirq_status,
					spi_write_byte,
					spi_read_byte);
	if (SI4455_init(&si4455,
					rx_cplt,
					tx_cplt,
					crc_err,
					0,
					&si4455_hal) == false)
	{
		assert_true(false);
	}
	spiInitSpeed(5000000);

	uint32_t syncword;
	if(SI4455_get_syncword(&si4455, (uint8_t*)&syncword) == false) {
		assert_true(false);
	}
	debugLogInfo("Sync word: 0x%08X", syncword);
	debugLogInfo("Channel: %d", si4455.channel);

#ifdef USE_RX
	if(SI4455_start_rx(&si4455) == false)
	{
		assert_true(false);
	}
	is_rx = false;
#else
	si4455_send("init msg");
#endif
}

void SI4455_TEST_run(void)
{
	if(SI4455_handle(&si4455)== false) {
		assert_true(false);
	}

#ifdef USE_RX
	if(is_rx) {
		is_rx = false;
		if(SI4455_start_rx(&si4455) == false)
		{
			assert_true(false);
		}
	}
#else
	si4455_send("Send msg loop");
#endif
}
