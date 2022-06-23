#ifndef _PIN_CONFIG_H_
#define _PIN_CONFIG_H_

#include "em_gpio.h"

#define GPIO_RADAR_TX_ON_PORT       gpioPortA	
#define GPIO_RADAR_TX_ON_PIN        0
#define GPIO_DBG_CLK_PORT           gpioPortA	
#define GPIO_DBG_CLK_PIN            1
#define GPIO_DBG_DIO_PORT           gpioPortA	
#define GPIO_DBG_DIO_PIN            2
#define GPIO_RADAR_IF_Q_HG_PORT     gpioPortA	
#define GPIO_RADAR_IF_Q_HG_PIN      3
#define GPIO_RADAR_IF_I_HG_PORT     gpioPortA	
#define GPIO_RADAR_IF_I_HG_PIN      4
#define GPIO_RADAR_ENABLE_PORT      gpioPortA	
#define GPIO_RADAR_ENABLE_PIN       5
#define GPIO_EN_OPAMP_PORT          gpioPortA	
#define GPIO_EN_OPAMP_PIN           6
#define GPIO_I2C0_SCL_PORT          gpioPortA	
#define GPIO_I2C0_SCL_PIN           7
#define GPIO_I2C0_SDA_PORT          gpioPortA	
#define GPIO_I2C0_SDA_PIN           8
#define GPIO_LED_RED_PORT           gpioPortB	
#define GPIO_LED_RED_PIN            0
#define GPIO_USER_BUTTON_PORT       gpioPortB	
#define GPIO_USER_BUTTON_PIN        1
#define GPIO_LED_YELLOW_PORT        gpioPortB	
#define GPIO_LED_YELLOW_PIN         2
#define GPIO_LED_GREEN_PORT         gpioPortB	
#define GPIO_LED_GREEN_PIN          3
#define GPIO_SI4455_nIRQ_PORT       gpioPortB
#define GPIO_SI4455_nIRQ_PIN        4
#define GPIO_BAT_VOLT_PORT          gpioPortC	
#define GPIO_BAT_VOLT_PIN           0
#define GPIO_BOOT_ENABLE_PORT       gpioPortC	
#define GPIO_BOOT_ENABLE_PIN        1
#define GPIO_SI4455_SDN_PORT        gpioPortC	
#define GPIO_SI4455_SDN_PIN         2
#define GPIO_SPI0_CS_PORT           gpioPortC	
#define GPIO_SPI0_CS_PIN            3
#define GPIO_SPI0_MOSI_PORT         gpioPortC	
#define GPIO_SPI0_MOSI_PIN          4
#define GPIO_SPI0_MISO_PORT         gpioPortC	
#define GPIO_SPI0_MISO_PIN          5
#define GPIO_SPI0_CLK_PORT          gpioPortC	
#define GPIO_SPI0_CLK_PIN           6
#define GPIO_SI4455_GPIO1_PORT      gpioPortC	
#define GPIO_SI4455_GPIO1_PIN       7
#define GPIO_BUZZER_PORT            gpioPortD	
#define GPIO_BUZZER_PIN             0
#define GPIO_BUZZER_ENABLE_PORT     gpioPortD	
#define GPIO_BUZZER_ENABLE_PIN      1

//#define GPIO_DBG_RX_PORT            gpioPortD
//#define GPIO_DBG_RX_PIN             2
//#define GPIO_DBG_TX_PORT            gpioPortB
//#define GPIO_DBG_TX_PIN             3

#define GPIO_DBG_RX_PORT            GPIO_I2C0_SCL_PORT
#define GPIO_DBG_RX_PIN             GPIO_I2C0_SCL_PIN
#define GPIO_DBG_TX_PORT            GPIO_I2C0_SDA_PORT
#define GPIO_DBG_TX_PIN             GPIO_I2C0_SDA_PIN

#define GPIO_HTPA_ENABLE_PORT       gpioPortD	
#define GPIO_HTPA_ENABLE_PIN        3

#define GPIO_ATS_PORT				gpioPortD	//GPIO_DBG_RX_PORT
#define GPIO_ATS_PIN				2			// GPIO_DBG_RX_PIN

#define GPIO_BAT_VOLT_ADC		    iadcPosInputPortCPin0
#define GPIO_RADAR_I_ADC		    iadcPosInputPortAPin4
#define GPIO_RADAR_Q_ADC		    iadcPosInputPortAPin3

#endif /*_PIN_CONFIGS_H_*/
