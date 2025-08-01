/*
 ******************************************************************************
 * @file    usb_ll.c
 * @brief   This file contains code derived from STMicroelectronics LL USB driver.
 ******************************************************************************
 * @attention
 *
 * Portions of this file are adapted from:
 * - stm32h5xx_ll_usb.c
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "usb_ll.h"
#include "stm32h5xx_ll_rcc.h"
#include "stm32h5xx_ll_utils.h"
#include "mcu_init.h"

void USB_ll_init()
{
	uint32_t interrupt_bitmap = USB_CNTR_CTRM  | USB_CNTR_WKUPM |
			   USB_CNTR_SUSPM | USB_CNTR_ERRM |
			   USB_CNTR_SOFM | USB_CNTR_ESOFM |
			   USB_CNTR_RESETM | USB_CNTR_L1REQM;

	LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_HSI48);

	/* Enable VDDUSB */
	SET_BIT(PWR->USBSCR, PWR_USBSCR_USB33SV);

	/* Enable USB kernel clock */
	__IO __attribute__((unused)) uint32_t tmpreg;
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USBEN);
	tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_USBEN);

	/* Initialize USB global interrupt and its priority */
	NVIC_SetPriority(USB_DRD_FS_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), USB_DRD_FS_INTR_PRI, 0));
	NVIC_EnableIRQ(USB_DRD_FS_IRQn);

	/* Remove PDWN but keep USBRST */
	USB_DRD_FS->CNTR = USB_CNTR_USBRST;

	/* Wait tstartup*/
	LL_mDelay(100);

	/* Disable pull-up on DP line to disconnect from the bus*/
	USB_DRD_FS->BCDR = 0;

	/* Release USBRST */
	USB_DRD_FS->CNTR = 0;

	/* Clear pending interrupts */
	USB_DRD_FS->ISTR = 0U;

	/* Set device mode */
	USB_DRD_FS->CNTR &= ~USB_CNTR_HOST;

	/* Enable interrupts */
	USB_DRD_FS->CNTR = interrupt_bitmap;

	/* Enable pull-up on DP line */
	USB_DRD_FS->BCDR |= USB_BCDR_DPPU;
}

int USB_ActivateEndpoint(USB_DRD_TypeDef *USBx, USB_DRD_EPTypeDef *ep)
{
  int ret = 0;
  uint32_t wEpRegVal;

  wEpRegVal = USB_DRD_GET_CHEP(USBx, ep->num) & USB_EP_T_MASK;

  /* initialize Endpoint */
  switch (ep->type)
  {
    case EP_TYPE_CTRL:
      wEpRegVal |= USB_EP_CONTROL;
      break;

    case EP_TYPE_BULK:
      wEpRegVal |= USB_EP_BULK;
      break;

    case EP_TYPE_INTR:
      wEpRegVal |= USB_EP_INTERRUPT;
      break;

    case EP_TYPE_ISOC:
      wEpRegVal |= USB_EP_ISOCHRONOUS;
      break;

    default:
      ret = -1;
      break;
  }

  USB_DRD_SET_CHEP(USBx, ep->num, (wEpRegVal | USB_EP_VTRX | USB_EP_VTTX));

  USB_DRD_SET_CHEP_ADDRESS(USBx, ep->num, ep->num);

  if (ep->doublebuffer == 0U)
  {
    if (ep->is_in != 0U)
    {
      /*Set the endpoint Transmit buffer address */
    	USB_DRD_SET_CHEP_TX_ADDRESS(USBx, ep->num, ep->pmaadress);
    	USB_DRD_CLEAR_TX_DTOG(USBx, ep->num);

      if (ep->type != EP_TYPE_ISOC)
      {
        /* Configure NAK status for the Endpoint */
    	  USB_DRD_SET_CHEP_TX_STATUS(USBx, ep->num, USB_EP_TX_NAK);
      }
      else
      {
        /* Configure TX Endpoint to disabled state */
    	  USB_DRD_SET_CHEP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
      }
    }
    else
    {
      /* Set the endpoint Receive buffer address */
    	USB_DRD_SET_CHEP_RX_ADDRESS(USBx, ep->num, ep->pmaadress);

      /* Set the endpoint Receive buffer counter */
    	USB_DRD_SET_CHEP_RX_CNT(USBx, ep->num, ep->maxpacket);
    	USB_DRD_CLEAR_RX_DTOG(USBx, ep->num);

      if (ep->num == 0U)
      {
        /* Configure VALID status for EP0 */
    	  USB_DRD_SET_CHEP_RX_STATUS(USBx, ep->num, USB_EP_RX_VALID);
      }
      else
      {
        /* Configure NAK status for OUT Endpoint */
    	  USB_DRD_SET_CHEP_RX_STATUS(USBx, ep->num, USB_EP_RX_NAK);
      }
    }
  }
#if (USE_USB_DOUBLE_BUFFER == 1U)
  /* Double Buffer */
  else
  {
    if (ep->type == EP_TYPE_BULK)
    {
      /* Set bulk endpoint as double buffered */
    	USB_DRD_SET_CHEP_KIND(USBx, ep->num);
    }
    else
    {
      /* Set the ISOC endpoint in double buffer mode */
    	USB_DRD_CLEAR_CHEP_KIND(USBx, ep->num);
    }

    /* Set buffer address for double buffered mode */
    USB_DRD_SET_CHEP_DBUF_ADDR(USBx, ep->num, ep->pmaaddr0, ep->pmaaddr1);

    if (ep->is_in == 0U)
    {
      /* Clear the data toggle bits for the endpoint IN/OUT */
    	USB_DRD_CLEAR_RX_DTOG(USBx, ep->num);
    	USB_DRD_CLEAR_TX_DTOG(USBx, ep->num);

    	USB_DRD_SET_CHEP_RX_STATUS(USBx, ep->num, USB_EP_RX_VALID);
    	USB_DRD_SET_CHEP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
    }
    else
    {
      /* Clear the data toggle bits for the endpoint IN/OUT */
    	USB_DRD_CLEAR_RX_DTOG(USBx, ep->num);
    	USB_DRD_CLEAR_TX_DTOG(USBx, ep->num);

      if (ep->type != EP_TYPE_ISOC)
      {
        /* Configure NAK status for the Endpoint */
    	  USB_DRD_SET_CHEP_TX_STATUS(USBx, ep->num, USB_EP_TX_NAK);
      }
      else
      {
        /* Configure TX Endpoint to disabled state */
    	  USB_DRD_SET_CHEP_TX_STATUS(USBx, ep->num, USB_EP_TX_DIS);
      }

      USB_DRD_SET_CHEP_RX_STATUS(USBx, ep->num, USB_EP_RX_DIS);
    }
  }
#endif /* (USE_USB_DOUBLE_BUFFER == 1U) */

  return ret;
}

/**
  * @brief Copy a buffer from user memory area to packet memory area (PMA)
  * @param   USBx USB peripheral instance register address.
  * @param   pbUsrBuf pointer to user memory area.
  * @param   wPMABufAddr address into PMA.
  * @param   wNBytes no. of bytes to be copied.
  * @retval None
  */
void USB_WritePMA(USB_DRD_TypeDef const *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes)
{
  (void)(USBx);
  uint32_t WrVal;
  uint32_t count;
  __IO uint32_t *pdwVal;
  uint32_t NbWords = ((uint32_t)wNBytes + 3U) >> 2U;
  /* Due to the PMA access 32bit only so the last non word data should be processed alone */
  uint16_t remaining_bytes = wNBytes % 4U;
  uint8_t *pBuf = pbUsrBuf;

  /* Check if there is a remaining byte */
  if (remaining_bytes != 0U)
  {
    NbWords--;
  }

  /* Get the PMA Buffer pointer */
  pdwVal = (__IO uint32_t *)(USB_DRD_PMAADDR + (uint32_t)wPMABufAddr);

  /* Write the Calculated Word into the PMA related Buffer */
  for (count = NbWords; count != 0U; count--)
  {
    *pdwVal = __UNALIGNED_UINT32_READ(pBuf);
    pdwVal++;
    /* Increment pBuf 4 Time as Word Increment */
    pBuf++;
    pBuf++;
    pBuf++;
    pBuf++;
  }

  /* When Number of data is not word aligned, write the remaining Byte */
  if (remaining_bytes != 0U)
  {
    WrVal = 0U;

    do
    {
      WrVal |= (uint32_t)(*(uint8_t *)pBuf) << (8U * count);
      count++;
      pBuf++;
      remaining_bytes--;
    } while (remaining_bytes != 0U);

    *pdwVal = WrVal;
  }
}

/**
  * @brief Copy data from packet memory area (PMA) to user memory buffer
  * @param   USBx USB peripheral instance register address.
  * @param   pbUsrBuf pointer to user memory area.
  * @param   wPMABufAddr address into PMA.
  * @param   wNBytes no. of bytes to be copied.
  * @retval None
  */
void USB_ReadPMA(USB_DRD_TypeDef const *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes)
{
  (void)(USBx);
  uint32_t count;
  uint32_t RdVal;
  __IO uint32_t *pdwVal;
  uint32_t NbWords = ((uint32_t)wNBytes + 3U) >> 2U;
  /*Due to the PMA access 32bit only so the last non word data should be processed alone */
  uint16_t remaining_bytes = wNBytes % 4U;
  uint8_t *pBuf = pbUsrBuf;

  /* Get the PMA Buffer pointer */
  pdwVal = (__IO uint32_t *)(USB_DRD_PMAADDR + (uint32_t)wPMABufAddr);

  /* if nbre of byte is not word aligned decrement the nbre of word*/
  if (remaining_bytes != 0U)
  {
    NbWords--;
  }

  /*Read the Calculated Word From the PMA related Buffer*/
  for (count = NbWords; count != 0U; count--)
  {
    __UNALIGNED_UINT32_WRITE(pBuf, *pdwVal);

    pdwVal++;
    pBuf++;
    pBuf++;
    pBuf++;
    pBuf++;
  }

  /*When Number of data is not word aligned, read the remaining byte*/
  if (remaining_bytes != 0U)
  {
    RdVal = *(__IO uint32_t *)pdwVal;

    do
    {
      *(uint8_t *)pBuf = (uint8_t)(RdVal >> (8U * (uint8_t)(count)));
      count++;
      pBuf++;
      remaining_bytes--;
    } while (remaining_bytes != 0U);
  }
}
