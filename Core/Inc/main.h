/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LAB_SW0_Pin GPIO_PIN_2
#define LAB_SW0_GPIO_Port GPIOE
#define SEG_DIG2_Pin GPIO_PIN_3
#define SEG_DIG2_GPIO_Port GPIOE
#define LAB_SW1_Pin GPIO_PIN_4
#define LAB_SW1_GPIO_Port GPIOE
#define LAB_SW2_Pin GPIO_PIN_5
#define LAB_SW2_GPIO_Port GPIOE
#define LAB_SW3_Pin GPIO_PIN_6
#define LAB_SW3_GPIO_Port GPIOE
#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define USER_Btn_EXTI_IRQn EXTI15_10_IRQn
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define LTA_SEG7_Pin GPIO_PIN_13
#define LTA_SEG7_GPIO_Port GPIOF
#define LTA_SEG8_Pin GPIO_PIN_14
#define LTA_SEG8_GPIO_Port GPIOF
#define CE_Pin GPIO_PIN_15
#define CE_GPIO_Port GPIOF
#define LTA_SEG9_Pin GPIO_PIN_0
#define LTA_SEG9_GPIO_Port GPIOG
#define LTA_SEG0_Pin GPIO_PIN_7
#define LTA_SEG0_GPIO_Port GPIOE
#define LTA_SEG1_Pin GPIO_PIN_8
#define LTA_SEG1_GPIO_Port GPIOE
#define LAB_PB0_CMD_Pin GPIO_PIN_9
#define LAB_PB0_CMD_GPIO_Port GPIOE
#define LAB_PB0_CMD_EXTI_IRQn EXTI9_5_IRQn
#define LTA_SEG2_Pin GPIO_PIN_10
#define LTA_SEG2_GPIO_Port GPIOE
#define LAB_PB3_EXE_Pin GPIO_PIN_11
#define LAB_PB3_EXE_GPIO_Port GPIOE
#define LAB_PB3_EXE_EXTI_IRQn EXTI15_10_IRQn
#define LTA_SEG3_Pin GPIO_PIN_12
#define LTA_SEG3_GPIO_Port GPIOE
#define LTA_SEG4_Pin GPIO_PIN_13
#define LTA_SEG4_GPIO_Port GPIOE
#define LTA_SEG5_Pin GPIO_PIN_14
#define LTA_SEG5_GPIO_Port GPIOE
#define LTA_SEG6_Pin GPIO_PIN_15
#define LTA_SEG6_GPIO_Port GPIOE
#define CSN_Pin GPIO_PIN_12
#define CSN_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define SEG_DIG0_Pin GPIO_PIN_2
#define SEG_DIG0_GPIO_Port GPIOG
#define SEG_DIG1_Pin GPIO_PIN_3
#define SEG_DIG1_GPIO_Port GPIOG
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define USB_SOF_Pin GPIO_PIN_8
#define USB_SOF_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SEG_A_Pin GPIO_PIN_0
#define SEG_A_GPIO_Port GPIOD
#define SEG_B_Pin GPIO_PIN_1
#define SEG_B_GPIO_Port GPIOD
#define SEG_C_Pin GPIO_PIN_2
#define SEG_C_GPIO_Port GPIOD
#define SEG_D_Pin GPIO_PIN_3
#define SEG_D_GPIO_Port GPIOD
#define SEG_E_Pin GPIO_PIN_4
#define SEG_E_GPIO_Port GPIOD
#define SEG_F_Pin GPIO_PIN_5
#define SEG_F_GPIO_Port GPIOD
#define SEG_G_Pin GPIO_PIN_6
#define SEG_G_GPIO_Port GPIOD
#define SEG_DP_Pin GPIO_PIN_7
#define SEG_DP_GPIO_Port GPIOD
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
