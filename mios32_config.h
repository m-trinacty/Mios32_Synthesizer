// $Id: mios32_config.h 1171 2011-04-10 18:58:03Z tk $
/*
 * Local MIOS32 configuration file
 *
 * this file allows to disable (or re-configure) default functions of MIOS32
 * available switches are listed in $MIOS32_PATH/modules/mios32/MIOS32_CONFIG.txt
 *
 */

#ifndef _MIOS32_CONFIG_H
#define _MIOS32_CONFIG_H

// The boot message which is print during startup and returned on a SysEx query
#define MIOS32_LCD_BOOT_MSG_LINE1 "Synthesizer"
#define MIOS32_LCD_BOOT_MSG_LINE2 "MIOS32 Synth"
#define MIOS32_LCD_BOOT_MSG_DELAY 2000

#if defined(MIOS32_FAMILY_STM32F10x)
// I2S device connected to J8 (-> SPI1), therefore we have to use SPI0 (-> J16) for SRIO chain
# define MIOS32_SRIO_SPI 0
#endif

// I2S support has to be enabled explicitely
#define MIOS32_USE_I2S

// enable MCLK pin (not for STM32 primer)
#ifdef MIOS32_BOARD_STM32_PRIMER
# define MIOS32_I2S_MCLK_ENABLE  0
#else
# define MIOS32_I2S_MCLK_ENABLE  1
#endif

#define MIOS32_UART0_TX_PORT     GPIOA
#define MIOS32_UART0_TX_PIN      GPIO_Pin_2
#define MIOS32_UART0_RX_PORT     GPIOA
#define MIOS32_UART0_RX_PIN      GPIO_Pin_3
#define MIOS32_UART0             USART2
#define MIOS32_UART0_IRQ_CHANNEL USART2_IRQn
#define MIOS32_UART0_IRQHANDLER_FUNC void USART2_IRQHandler(void)
#define MIOS32_UART0_REMAP_FUNC  { GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); }
#define MIOS32_IIC_MIDI_NUM 1

#define MIOS32_AIN_CHANNEL_MASK 0x0011

// define the deadband (min. difference to report a change to the application hook)
// typically set to (2^(12-desired_resolution)-1)
// e.g. for a resolution of 7 bit, it's set to (2^(12-7)-1) = (2^5 - 1) = 31
#define MIOS32_AIN_DEADBAND 31


#endif /* _MIOS32_CONFIG_H */
