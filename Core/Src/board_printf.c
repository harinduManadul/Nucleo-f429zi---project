#include "board_printf.h"



UART_HandleTypeDef brd_printfUartHandle;


void board_printf_init() {

  GPIO_InitTypeDef  GPIO_InitStruct;


  //CLK_ENABLE();
  BRD_PRINTF_USARTx_RX_GPIO_CLK_ENABLE();


  /* Enable USARTx clock */
  BRD_PRINTF_USARTx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = BRD_PRINTF_USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = BRD_PRINTF_USARTx_TX_AF;

  HAL_GPIO_Init(BRD_PRINTF_USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = BRD_PRINTF_USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = BRD_PRINTF_USARTx_RX_AF;

  HAL_GPIO_Init(BRD_PRINTF_USARTx_RX_GPIO_PORT, &GPIO_InitStruct);


  brd_printfUartHandle.Instance        = BRD_PRINTF_UART;
  brd_printfUartHandle.Init.BaudRate   = 115200;
  brd_printfUartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  brd_printfUartHandle.Init.StopBits   = UART_STOPBITS_1;
  brd_printfUartHandle.Init.Parity     = UART_PARITY_NONE;
  brd_printfUartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  brd_printfUartHandle.Init.Mode       = UART_MODE_TX_RX;
  brd_printfUartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&brd_printfUartHandle) != HAL_OK)
  {
    /* Initialization Error */
    while (1)
    {
    }
  }

}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&brd_printfUartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}