/**
 ******************************************************************************
 * @file    nrf24l01plus_hal_tx.c
 * @author  MDS, modified by Jack Cairns
 * @brief   nrf24l01+ driver for the STM32F4 using the HAL. Owns its SPI
 *          peripheral, CS/CE pins and a microsecond delay timer (see
 *          nrf24l01plus_hal_tx.h to change them). Polled only, no IRQ pin.
 *          Radio setup: CRC off, auto-ack off, 1Mbps, 0dBm, 32 byte payload.
 ******************************************************************************
 */

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "nrf24l01plus_hal_tx.h"

#ifdef NRF_DEBUG

#include "board_printf.h"
#define NRF_DEBUG_PRINTF(...) printf(__VA_ARGS__)
#define NRF_DEBUG_INIT()      board_printf_init()

#else

#define NRF_NOP()                                                                                  \
    do {                                                                                           \
    } while (0)

#define NRF_DEBUG_PRINTF(...) NRF_NOP()
#define NRF_DEBUG_INIT()      NRF_NOP()

#endif

// Set default channel and addreess from either a file or directly set.
#ifdef MYCONFIG

#include "myconfig.h"
#define DEFAULT_RF_CHANNEL MYRADIOCHAN
#define default_addr       myradiotxaddr

#else

#define DEFAULT_RF_CHANNEL 40 // Default radio channel
static uint8_t default_addr[] = {0x12, 0x34, 0x56, 0x78, 0x90}; // Default address with MSB first

#endif

// CONFIG register values: all IRQs masked, CRC disabled, PWR_UP set
#define NRF_CONFIG_TX 0x72 // PRIM_RX = 0

#define NRF_TX_TIMEOUT_US 4500

static TIM_HandleTypeDef nrfTimHandle;
static SPI_HandleTypeDef nrfSpiHandle;

/**
 * @brief  Busy-wait delay using the driver's free running 1MHz timer.
 *         Safe to call from inside a critical section (does not use the tick).
 * @param  us Number of microseconds to wait.
 * @retval None
 */
static void delay_us(uint32_t us)
{
    uint32_t start_tick = __HAL_TIM_GET_COUNTER(&nrfTimHandle);
    while ((__HAL_TIM_GET_COUNTER(&nrfTimHandle) - start_tick) < us) {
        // idle...
    }
}

/**
 * @brief  Enable the GPIO clocks and configure the SPI (SCK, MISO, MOSI), CS and
 *         CE pins. CS is left high (deselected) and CE low (standby).
 * @param  None
 * @retval None
 */
static void nrf_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*
        SPI2 GPIO Configuration Default
        PC2     ------> SPI2_MISO
        PC3     ------> SPI2_MOSI
        PB10    ------> SPI2_SCK
        PB12    ------> SPI2_CS
        PF15    ------> NRF_MODE
    */

    // NRF SPI Chip select Pin
    __NRF_SPI_CS_GPIO_CLK();
    GPIO_InitStruct.Pin = NRF_SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(NRF_SPI_CS_GPIO_PORT, &GPIO_InitStruct);
    NRF_CS_HIGH();

    // SPI CLK PIN setup
    __NRF_SPI_SCK_GPIO_CLK(); // Clock enable
    GPIO_InitStruct.Pin = NRF_SPI_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = NRF_SPI_SCK_AF;
    HAL_GPIO_Init(NRF_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

    // SPI MISO PIN setup
    __NRF_SPI_MISO_GPIO_CLK(); // Clock enable
    GPIO_InitStruct.Pin = NRF_SPI_MISO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = NRF_SPI_MISO_AF;
    HAL_GPIO_Init(NRF_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

    // SPI MOSI PIN setup
    __NRF_SPI_MOSI_GPIO_CLK(); // Clock enable
    GPIO_InitStruct.Pin = NRF_SPI_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = NRF_SPI_MOSI_AF;
    HAL_GPIO_Init(NRF_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

    // NRF Chip Mode Pin setup (CE in data sheet)
    __NRF_MODE_GPIO_CLK();
    GPIO_InitStruct.Pin = NRF_MODE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(NRF_MODE_GPIO_PORT, &GPIO_InitStruct);
    NRF_CE_LOW();
}

/**
 * @brief  Enable the SPI peripheral clock and configure it as an 8-bit, mode 0
 *         (CPOL = 0, CPHA = 0), MSB first master with software chip select.
 * @param  None
 * @retval None
 */
static void nrf_SPI_Init(void)
{
    __NRF_SPI_CLK();
    nrfSpiHandle.Instance = NRF_SPI;
    nrfSpiHandle.Init.Mode = SPI_MODE_MASTER;
    nrfSpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    nrfSpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    nrfSpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    nrfSpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    nrfSpiHandle.Init.NSS = SPI_NSS_SOFT;
    nrfSpiHandle.Init.BaudRatePrescaler = NRF_SPI_BAUDRATEPRESCALER;
    nrfSpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    nrfSpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    nrfSpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    nrfSpiHandle.Init.CRCPolynomial = 10; // Not used
    if (HAL_SPI_Init(&nrfSpiHandle) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  Configure and start the delay timer as a free running 32-bit counter
 *         ticking at 1MHz. The prescaler is calculated from the APB1 timer clock.
 * @param  None
 * @retval None
 */
static void nrf_TIM_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    __NRF_TIM_CLK();
    uint32_t timClk = HAL_RCC_GetPCLK1Freq();
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_HCLK_DIV1) {
        timClk *= 2;
    }

    nrfTimHandle.Instance = NRF_TIM;
    nrfTimHandle.Init.Prescaler = (timClk / 1000000) - 1; // 1MHz -> 1 tick per us
    nrfTimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    nrfTimHandle.Init.Period = 0xFFFFFFFF;
    nrfTimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    nrfTimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&nrfTimHandle) != HAL_OK) {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&nrfTimHandle, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&nrfTimHandle, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    // Free running counter, no interrupts needed
    if (HAL_TIM_Base_Start(&nrfTimHandle) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  Clock one byte out on SPI and return the byte clocked in.
 *         Does not touch CS, the caller must hold CS low for the whole command.
 * @param  byte Byte to transmit.
 * @retval Byte received from the nrf.
 */
static uint8_t nrf24l01plus_hal_spi_SendRecv(uint8_t byte)
{
    uint8_t rxbyte = 0;
    HAL_SPI_TransmitReceive(&nrfSpiHandle, &byte, &rxbyte, 1, HAL_MAX_DELAY);
    return rxbyte;
}

/**
 * @brief  Send a single byte command (e.g. FLUSH_TX, FLUSH_RX) with its own
 *         CS low period.
 * @param  command Command byte.
 * @retval None
 */
static void nrf24l01plus_hal_cmd(uint8_t command)
{
    NRF_CS_LOW();
    nrf24l01plus_hal_spi_SendRecv(command);
    NRF_CS_HIGH();

    NRF_DEBUG_PRINTF("NRF DEBUG: CMD 0x%02X\r\n", command);
}

/**
 * @brief  Write a single byte register.
 * @param  reg_addr Register address (WRITE_REG is added internally).
 * @param  val      Value to write.
 * @retval None
 */
static void nrf24l01plus_hal_wr(uint8_t reg_addr, uint8_t val)
{
    NRF_CS_LOW();
    nrf24l01plus_hal_spi_SendRecv(NRF24L01P_WRITE_REG | reg_addr);
    nrf24l01plus_hal_spi_SendRecv(val);
    NRF_CS_HIGH();

    NRF_DEBUG_PRINTF("NRF DEBUG: WR reg 0x%02X = 0x%02X\r\n", reg_addr, val);
}

/**
 * @brief  Read a single byte register.
 * @param  reg_addr Register address.
 * @retval Register value.
 */
static uint8_t nrf24l01plus_hal_rr(uint8_t reg_addr)
{
    NRF_CS_LOW();
    nrf24l01plus_hal_spi_SendRecv(NRF24L01P_READ_REG | reg_addr);
    uint8_t rxbyte = nrf24l01plus_hal_spi_SendRecv(NRF24L01P_OP_NOP);
    NRF_CS_HIGH();
    return rxbyte;
}

/**
 * @brief  Send a command byte followed by a buffer of data in one CS low period,
 *         e.g. (WRITE_REG | TX_ADDR) for an address or WR_TX_PLOAD for a payload.
 * @param  command    Command byte, including WRITE_REG for register writes.
 * @param  buffer     Data to send.
 * @param  buffer_len Number of bytes in buffer.
 * @retval None
 */
static void nrf24l01plus_hal_wb(uint8_t command, uint8_t *buffer, int buffer_len)
{
    NRF_CS_LOW();
    nrf24l01plus_hal_spi_SendRecv(command);
    for (int i = 0; i < buffer_len; i++) {
        nrf24l01plus_hal_spi_SendRecv(buffer[i]);
    }

    NRF_CS_HIGH();

    // Printed after CS goes high so printing doesn't stretch the SPI transfer
#ifdef NRF_DEBUG
    NRF_DEBUG_PRINTF("NRF DEBUG: WB 0x%02X:", command);
    for (int i = 0; i < buffer_len; i++) {
        NRF_DEBUG_PRINTF(" %02X", buffer[i]);
    }

    NRF_DEBUG_PRINTF("\r\n");
#endif
}

/**
 * @brief  Send a command byte then read the data that follows in one CS low
 *         period, e.g. RD_RX_PLOAD to read a payload.
 * @param  command    Command byte.
 * @param  buffer     Destination for the received data.
 * @param  buffer_len Number of bytes to read.
 * @retval None
 */
static __unused void nrf24l01plus_hal_rb(uint8_t command, uint8_t *buffer, int buffer_len)
{
    NRF_CS_LOW();
    nrf24l01plus_hal_spi_SendRecv(command);
    for (int i = 0; i < buffer_len; i++) {
        buffer[i] = nrf24l01plus_hal_spi_SendRecv(NRF24L01P_OP_NOP);
    }

    NRF_CS_HIGH();

#ifdef NRF_DEBUG
    NRF_DEBUG_PRINTF("NRF DEBUG: RB 0x%02X:", command);
    for (int i = 0; i < buffer_len; i++) {
        NRF_DEBUG_PRINTF(" %02X", buffer[i]);
    }

    NRF_DEBUG_PRINTF("\r\n");
#endif
}

/**
 * @brief  Initialise the timer, pins, SPI and nrf24l01+, leaving the radio in
 *         transmit standby. See nrf24l01plus_hal_tx.h for details.
 */
void nrf24l01plus_hal_init(void)
{

    NRF_DEBUG_INIT();

    nrf_TIM_Init();
    nrf_GPIO_Init();
    nrf_SPI_Init();

    // nrf power on reset takes up to 100ms from VCC
    delay_us(100000);

    // Power up (standby-I) and wait for the crystal to settle (Tpd2stby = 1.5ms)
    nrf24l01plus_hal_wr(NRF24L01P_CONFIG, NRF_CONFIG_TX);
    delay_us(5000);

    // Writes TX_Address and RX_Address (pipe 0) to nRF24L01
    nrf24l01plus_hal_wb(NRF24L01P_WRITE_REG | NRF24L01P_TX_ADDR, default_addr,
                        NRF24L01P_TX_ADR_WIDTH);
    nrf24l01plus_hal_wb(NRF24L01P_WRITE_REG | NRF24L01P_RX_ADDR_P0, default_addr,
                        NRF24L01P_TX_ADR_WIDTH);

    // Disable Auto.Ack (CRC is forced on otherwise)
    nrf24l01plus_hal_wr(NRF24L01P_EN_AA, 0x00);
    // Enable Pipe0
    nrf24l01plus_hal_wr(NRF24L01P_EN_RXADDR, 0x01);
    // Select same RX payload width as TX Payload width
    nrf24l01plus_hal_wr(NRF24L01P_RX_PW_P0, NRF24L01P_TX_PLOAD_WIDTH);

    // Select RF channel
    nrf24l01plus_hal_wr(NRF24L01P_RF_CH, DEFAULT_RF_CHANNEL);
    // TX_PWR:0dBm, Datarate:1Mbps
    nrf24l01plus_hal_wr(NRF24L01P_RF_SETUP, 0x06);

    // Clear any pending flags and empty the TX FIFO
    nrf24l01plus_hal_wr(NRF24L01P_STATUS, NRF24L01P_RX_DR | NRF24L01P_TX_DS | NRF24L01P_MAX_RT);
    nrf24l01plus_hal_flush_tx();

    // Read back the key registers so an SPI/wiring fault is obvious
    NRF_DEBUG_PRINTF("NRF DEBUG: init CONFIG 0x%02X (expect 0x%02X) RF_CH %u (expect %u) "
                     "RF_SETUP 0x%02X (expect 0x06) STATUS 0x%02X FIFO 0x%02X\r\n",
                     nrf24l01plus_hal_rr(NRF24L01P_CONFIG), NRF_CONFIG_TX,
                     nrf24l01plus_hal_rr(NRF24L01P_RF_CH), DEFAULT_RF_CHANNEL,
                     nrf24l01plus_hal_rr(NRF24L01P_RF_SETUP), nrf24l01plus_hal_rr(NRF24L01P_STATUS),
                     nrf24l01plus_hal_rr(NRF24L01P_FIFO_STATUS));
}


/**
 * @brief  Switch the nrf to transmit standby (CE low, PRIM_RX = 0).
 */
void nrf24l01plus_hal_send_mode(void)
{
    NRF_CE_LOW();
    nrf24l01plus_hal_wr(NRF24L01P_CONFIG, NRF_CONFIG_TX);
}

/**
 * @brief  Transmit one payload and wait for it to leave, leaving the radio in
 *         transmit standby. See nrf24l01plus_hal_tx.h for details.
 * @retval HAL_OK if sent, HAL_TIMEOUT if TX_DS was not set within NRF_TX_TIMEOUT_US.
 */
uint8_t nrf24l01plus_hal_send(uint8_t *tx_buf)
{
    uint8_t error = HAL_OK;
    nrf24l01plus_hal_send_mode();
    // write playload to TX_FIFO
    nrf24l01plus_hal_wb(NRF24L01P_WR_TX_PLOAD, tx_buf, NRF24L01P_TX_PLOAD_WIDTH);

    NRF_CE_LOW();
    delay_us(20);
    NRF_CE_HIGH(); // Pulse the CE pin for > 10us
    delay_us(20);
    NRF_CE_LOW();

    // Wait for the packet to leave. TX_DS is still set in STATUS while masked in CONFIG.
    uint32_t waited = 0;
    while (!(nrf24l01plus_hal_rr(NRF24L01P_STATUS) & NRF24L01P_TX_DS) &&
           waited < NRF_TX_TIMEOUT_US) {
        delay_us(10);
        waited += 10;
    }

#ifdef NRF_DEBUG
    uint8_t fifoAfterTx = nrf24l01plus_hal_rr(NRF24L01P_FIFO_STATUS);
#endif

    if (waited >= NRF_TX_TIMEOUT_US) {
        NRF_DEBUG_PRINTF("NRF DEBUG: send TIMEOUT, STATUS 0x%02X FIFO 0x%02X\r\n",
                         nrf24l01plus_hal_rr(NRF24L01P_STATUS), fifoAfterTx);
        nrf24l01plus_hal_flush_tx(); // Never sent, don't leave it in the FIFO
        error = HAL_TIMEOUT;
    } else {
        NRF_DEBUG_PRINTF("NRF DEBUG: send TX_DS after ~%luus, FIFO 0x%02X%s\r\n",
                         (unsigned long)waited, fifoAfterTx,
                         (fifoAfterTx & NRF24L01P_FIFO_TX_EMPTY) ? "" : " (TX FIFO NOT EMPTY)");
    }

    nrf24l01plus_hal_wr(NRF24L01P_STATUS, NRF24L01P_TX_DS); // Clear TX_DS
    return error;
}


/**
 * @brief  Check whether the TX FIFO is empty.
 */
uint8_t nrf24l01plus_hal_txFifoEmpty(void)
{
    uint8_t fifoStatus = nrf24l01plus_hal_rr(NRF24L01P_FIFO_STATUS);
    NRF_DEBUG_PRINTF("NRF DEBUG: FIFO_STATUS 0x%02X\r\n", fifoStatus);
    return !!(fifoStatus & NRF24L01P_FIFO_TX_EMPTY);
}


/**
 * @brief  Discard everything in the TX FIFO.
 */
void nrf24l01plus_hal_flush_tx(void)
{
    nrf24l01plus_hal_cmd(NRF24L01P_FLUSH_TX);
}
