/*
 * This file contains source code licensed under the BSD 3-Clause License.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the full BSD 3-Clause license
 *    notice, including copyright.
 * 2. Redistributions in binary form must reproduce the full license notice in
 *    the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the original author(s) nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * This file is provided "as is" without any express or implied warranties.
 * See the LICENSE file for full license details.
 */

#ifndef MCU_INIT_H_
#define MCU_INIT_H_

#include "stm32h5xx.h"
#include "stm32h5xx_ll_pwr.h"
#include "stm32h5xx_ll_crs.h"
#include "stm32h5xx_ll_rcc.h"
#include "stm32h5xx_ll_bus.h"
#include "stm32h5xx_ll_system.h"
#include "stm32h5xx_ll_exti.h"
#include "stm32h5xx_ll_cortex.h"
#include "stm32h5xx_ll_utils.h"
#include "stm32h5xx_ll_tim.h"
#include "stm32h5xx_ll_usart.h"
#include "stm32h5xx_ll_gpio.h"
#include "stm32h5xx_ll_tim.h"

#define USER_BUTTON_Pin 		LL_GPIO_PIN_13
#define USER_BUTTON_GPIO_Port 	GPIOC
#define USER_BUTTON_EXTI_IRQn 	EXTI13_IRQn

#define LED2_Pin 				LL_GPIO_PIN_4
#define LED2_GPIO_Port 			GPIOF
#define LED1_Pin 				LL_GPIO_PIN_0
#define LED1_GPIO_Port 			GPIOB
#define LED3_Pin 				LL_GPIO_PIN_4
#define LED3_GPIO_Port 			GPIOG

#define USART_TX_Pin 			LL_GPIO_PIN_6
#define USART_TX_GPIO_Port 		GPIOB
#define USART_RX_Pin 			LL_GPIO_PIN_7
#define USART_RX_GPIO_Port 		GPIOB

/* Virtual COM port */
#define VCP_TX_Pin 				LL_GPIO_PIN_8
#define VCP_TX_GPIO_Port 		GPIOD
#define VCP_RX_Pin 				LL_GPIO_PIN_9
#define VCP_RX_GPIO_Port 		GPIOD

#define USB_FS_N_Pin 			LL_GPIO_PIN_11
#define USB_FS_N_GPIO_Port 		GPIOA
#define USB_FS_P_Pin 			LL_GPIO_PIN_12
#define USB_FS_P_GPIO_Port 		GPIOA

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

#define USB_DRD_FS_INTR_PRI			2
#define TICK_INT_PRIORITY         	3

extern uint64_t sys_tick;

int mcu_init();
void delay_us(uint32_t d_us);

#endif /* MCU_INIT_H_ */
