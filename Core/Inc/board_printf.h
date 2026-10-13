#ifndef BOARD_PRINTF
#define BOARD_PRINTF

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifndef BRD_PRINTF_UART
#define BRD_PRINTF_UART USART3
#endif

#define BRD_PRINTF_USARTx                           USART3
#define BRD_PRINTF_USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE();
#define BRD_PRINTF_USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define BRD_PRINTF_USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()

#define BRD_PRINTF_USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define BRD_PRINTF_USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USARTx Pins */
#define BRD_PRINTF_USARTx_TX_PIN                    GPIO_PIN_8
#define BRD_PRINTF_USARTx_TX_GPIO_PORT              GPIOD
#define BRD_PRINTF_USARTx_TX_AF                     GPIO_AF7_USART3
#define BRD_PRINTF_USARTx_RX_PIN                    GPIO_PIN_9
#define BRD_PRINTF_USARTx_RX_GPIO_PORT              GPIOD
#define BRD_PRINTF_USARTx_RX_AF                     GPIO_AF7_USART3

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */



void board_printf_init();

#endif