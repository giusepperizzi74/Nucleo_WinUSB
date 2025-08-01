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

#include "mcu_init.h"
#include "usb.h"

#define TIM5_PRESCALED_CLK_HZ     1000000 // used for delay_us()

uint64_t sys_tick;

static void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
	while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
	{
	}

	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
	while (LL_PWR_IsActiveFlag_VOS() == 0)
	{
	}
	LL_RCC_HSI48_Enable();

	/* Wait till HSI48 is ready */
	while(LL_RCC_HSI48_IsReady() != 1)
	{
	}

	LL_RCC_CSI_Enable();

	/* Wait till CSI is ready */
	while(LL_RCC_CSI_IsReady() != 1)
	{
	}

	LL_RCC_CSI_SetCalibTrimming(32);
	LL_RCC_PLL1_SetSource(LL_RCC_PLL1SOURCE_CSI);
	LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
	LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
	LL_RCC_PLL1_SetM(1);
	LL_RCC_PLL1_SetN(125);
	LL_RCC_PLL1_SetP(2);
	LL_RCC_PLL1_SetQ(2);
	LL_RCC_PLL1_SetR(2);
	LL_RCC_PLL1P_Enable();
	LL_RCC_PLL1_Enable();

	/* Wait till PLL is ready */
	while(LL_RCC_PLL1_IsReady() != 1)
	{
	}

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
	{
	}

	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

	LL_Init1msTick(250000000);

	/* Enable interrupt from SysTick */
	SET_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);

	LL_SetSystemCoreClock(250000000);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_CRS);
	LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_CRS);
	LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_CRS);
	LL_CRS_SetSyncDivider(LL_CRS_SYNC_DIV_1);
	LL_CRS_SetSyncPolarity(LL_CRS_SYNC_POLARITY_RISING);
	LL_CRS_SetSyncSignalSource(LL_CRS_SYNC_SOURCE_USB);
	LL_CRS_SetReloadCounter(__LL_CRS_CALC_CALCULATE_RELOADVALUE(48000000,1000));
	LL_CRS_SetFreqErrorLimit(34);
	LL_CRS_SetHSI48SmoothTrimming(32);
}

static void TIM5_Init(void)
{
	LL_TIM_DisableCounter(TIM5);
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);

	TIM_InitStruct.Prescaler = SystemCoreClock/TIM5_PRESCALED_CLK_HZ-1;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 4294967295;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM5, &TIM_InitStruct);
	LL_TIM_DisableARRPreload(TIM5);
	LL_TIM_SetClockSource(TIM5, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetTriggerOutput(TIM5, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM5);

	LL_TIM_EnableCounter(TIM5);
}

static void USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
  /**USART3 GPIO Configuration
  PD8   ------> USART3_TX
  PD9   ------> USART3_RX
  */
  GPIO_InitStruct.Pin = VCP_TX_Pin|VCP_RX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART3, &USART_InitStruct);
  LL_USART_SetTXFIFOThreshold(USART3, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(USART3, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_DisableFIFO(USART3);
  LL_USART_ConfigAsyncMode(USART3);
  LL_USART_Enable(USART3);
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

static void GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);

  /**/
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LED3_GPIO_Port, LED3_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_7);

  /**/
  LL_GPIO_SetOutputPin(GPIOD, LL_GPIO_PIN_0);

  /**/
  LL_EXTI_SetEXTISource(LL_EXTI_EXTI_PORTC, LL_EXTI_EXTI_LINE13);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
  EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_PULL_NO);

  /**/
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_MODE_INPUT);

  /*
   * Set all GPIOE ecept PE1 as input. The default (ANALOG INPUT) would prevent the set/clear/get functions from working
   */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5
                          |LL_GPIO_PIN_6|LL_GPIO_PIN_7|LL_GPIO_PIN_8|LL_GPIO_PIN_9
                          |LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13
                          |LL_GPIO_PIN_14|LL_GPIO_PIN_15|LL_GPIO_PIN_0;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED3_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED3_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI13_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));
  NVIC_EnableIRQ(EXTI13_IRQn);
}

int mcu_init()
{
	/* System interrupt init*/
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	/* SysTick_IRQn interrupt configuration */
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));

	/** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
	*/
	LL_PWR_DisableUCPDDeadBattery();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	GPIO_Init();
	TIM5_Init();
	USB_Init();
	USART3_UART_Init();

	return 0;
}

void delay_us(uint32_t d_us)
{
    uint32_t start = LL_TIM_GetCounter(TIM5);
    uint32_t end = start + d_us;
    if(end < start) // -> start + d caused an overflow
        while(LL_TIM_GetCounter(TIM5) > start) {}
    while(LL_TIM_GetCounter(TIM5) < end) {}
}
