/* -----------------------------------------------------------------------------
# Company name: CSSE3010 - Embedded Systems Design & Interfacing
# Project name: nRF24l01+ driver
# Module name : nrf24l01plus
# Functionality: Provides low level radio calls used to initialise and interface
#        with the nrf24l01+.
#
# Author name: Chris Rieger modified by MDS, Jack Cairns
# Creation date: 010712,
# Revision date (name): 210316, 240925 MDS, 270926
# Changes implemented (date): 210316 Renamed functions and modified for new spec
#(Comments):
------------------------------------------------------------------------------*/
#ifndef _NRF24l01PLUS_HAL_H
#define _NRF24l01PLUS_HAL_H

#ifdef HAVE_PLATFORMCONF_H
#include "platform-conf.h"
#endif

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "nrf24l01plus_reg.h"

/* Define Pins, if not specified by a platform-conf.h file */

/* Mode (CE) Pin */
#ifndef NRF_MODE_PIN
#define NRF_MODE_PIN GPIO_PIN_15
#endif

#ifndef NRF_MODE_GPIO_PORT
#define NRF_MODE_GPIO_PORT GPIOF
#endif

#ifndef __NRF_MODE_GPIO_CLK
#define __NRF_MODE_GPIO_CLK() __HAL_RCC_GPIOF_CLK_ENABLE()
#endif

/* SPI CS Pin */
#ifndef NRF_SPI_CS_PIN
#define NRF_SPI_CS_PIN GPIO_PIN_12
#endif

#ifndef NRF_SPI_CS_GPIO_PORT
#define NRF_SPI_CS_GPIO_PORT GPIOB
#endif

#ifndef __NRF_SPI_CS_GPIO_CLK
#define __NRF_SPI_CS_GPIO_CLK() __HAL_RCC_GPIOB_CLK_ENABLE()
#endif

/* SPI SCK Pin */
#ifndef NRF_SPI_SCK_PIN
#define NRF_SPI_SCK_PIN GPIO_PIN_10
#endif

#ifndef NRF_SPI_SCK_GPIO_PORT
#define NRF_SPI_SCK_GPIO_PORT GPIOB
#endif

#ifndef __NRF_SPI_SCK_GPIO_CLK
#define __NRF_SPI_SCK_GPIO_CLK() __HAL_RCC_GPIOB_CLK_ENABLE()
#endif

#ifndef NRF_SPI_SCK_AF
#define NRF_SPI_SCK_AF GPIO_AF5_SPI2
#endif

/* SPI MISO Pin */
#ifndef NRF_SPI_MISO_PIN
#define NRF_SPI_MISO_PIN GPIO_PIN_2
#endif

#ifndef NRF_SPI_MISO_GPIO_PORT
#define NRF_SPI_MISO_GPIO_PORT GPIOC
#endif

#ifndef __NRF_SPI_MISO_GPIO_CLK
#define __NRF_SPI_MISO_GPIO_CLK() __HAL_RCC_GPIOC_CLK_ENABLE()
#endif

#ifndef NRF_SPI_MISO_AF
#define NRF_SPI_MISO_AF GPIO_AF5_SPI2
#endif

/* SPI MOSI Pin */
#ifndef NRF_SPI_MOSI_PIN
#define NRF_SPI_MOSI_PIN GPIO_PIN_3
#endif

#ifndef NRF_SPI_MOSI_GPIO_PORT
#define NRF_SPI_MOSI_GPIO_PORT GPIOC
#endif

#ifndef __NRF_SPI_MOSI_GPIO_CLK
#define __NRF_SPI_MOSI_GPIO_CLK() __HAL_RCC_GPIOC_CLK_ENABLE()
#endif

#ifndef NRF_SPI_MOSI_AF
#define NRF_SPI_MOSI_AF GPIO_AF5_SPI2
#endif

/* SPI Peripheral */
#ifndef NRF_SPI
#define NRF_SPI SPI2
#endif

#ifndef __NRF_SPI_CLK
#define __NRF_SPI_CLK() __HAL_RCC_SPI2_CLK_ENABLE()
#endif

/* SPI clock must be <= 10MHz for the nrf. APB1 (42MHz) / 32 = 1.3125MHz */
#ifndef NRF_SPI_BAUDRATEPRESCALER
#define NRF_SPI_BAUDRATEPRESCALER SPI_BAUDRATEPRESCALER_32
#endif

/* Microsecond delay timer (32-bit, APB1). Must NOT also be enabled in CubeMX. */
#ifndef NRF_TIM
#define NRF_TIM TIM2
#endif

#ifndef __NRF_TIM_CLK
#define __NRF_TIM_CLK() __HAL_RCC_TIM2_CLK_ENABLE()
#endif

/* Set/Reset CE pin */
#define NRF_CE_HIGH() (NRF_MODE_GPIO_PORT->ODR |= NRF_MODE_PIN)
#define NRF_CE_LOW()  (NRF_MODE_GPIO_PORT->ODR &= ~NRF_MODE_PIN)

/* Set/Reset CS pin */
#define NRF_CS_HIGH() (NRF_SPI_CS_GPIO_PORT->ODR |= NRF_SPI_CS_PIN)
#define NRF_CS_LOW()  (NRF_SPI_CS_GPIO_PORT->ODR &= ~NRF_SPI_CS_PIN)

/**
 * @brief  Initialise the delay timer, GPIO pins and SPI, then configure the
 *         nrf24l01+ (address, channel, CRC off, auto-ack off, 1Mbps, 0dBm,
 *         32 byte payload on pipe 0), clear the status flags and flush the
 *         TX FIFO. Must be called once before any other nrf24l01plus_hal_
 *         function. Blocks for ~105ms using busy-wait delays only, so it is 
 *         safe to call before the scheduler starts or inside a critical 
 *         section.
 * @param  None
 * @retval None
 */
void nrf24l01plus_hal_init(void);

/**
 * @brief  Put the nrf into transmit standby: CE low and CONFIG = 0x72
 *         (PRIM_RX = 0). A CE pulse is then needed to send the TX FIFO.
 * @param  None
 * @retval None
 */
void nrf24l01plus_hal_send_mode(void);

/**
 * @brief  Send one payload. Switches to send mode, loads the TX FIFO, pulses CE,
 *         then polls STATUS for TX_DS for up to 4.5ms (NRF_TX_TIMEOUT_US in
 *         nrf24l01plus_hal_tx.c) and clears it. On a timeout the TX FIFO is
 *         flushed so the unsent payload isn't sent later.
 *
 *         Either way the radio is left in transmit standby.
 * @param  tx_buf Payload to send, NRF24L01P_TX_PLOAD_WIDTH (32) bytes.
 * @retval HAL_OK      Packet sent (TX_DS set).
 * @retval HAL_TIMEOUT TX_DS never set, packet not sent.
 */
uint8_t nrf24l01plus_hal_send(uint8_t *tx_buf);

/**
 * @brief  Check whether the TX FIFO is empty (FIFO_STATUS.TX_EMPTY).
 * @param  None
 * @retval 1 if empty, 0 otherwise.
 */
uint8_t nrf24l01plus_hal_txFifoEmpty(void);

/**
 * @brief  Discard every payload in the TX FIFO (FLUSH_TX command).
 * @param  None
 * @retval None
 */
void nrf24l01plus_hal_flush_tx(void);

#endif
