/*
 ******************************************************************************
 * @file    usb_ll.h
 * @brief   This file contains code derived from STMicroelectronics LL USB driver.
 ******************************************************************************
 * @attention
 *
 * Portions of this file are adapted from:
 * - stm32h5xx_ll_usb.h
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

#ifndef INC_USB_LL_H_
#define INC_USB_LL_H_

#include "stm32h5xx.h"
#include "stm32h563xx.h"

/**
  * @brief  USB Instance Initialization Structure definition
  */
typedef struct
{
  uint8_t dev_endpoints;            /*!< Device Endpoints number.
                                         This parameter depends on the used USB core.
                                         This parameter must be a number between Min_Data = 1 and Max_Data = 15 */

  uint8_t Host_channels;            /*!< Host Channels number.
                                         This parameter Depends on the used USB core.
                                         This parameter must be a number between Min_Data = 1 and Max_Data = 15 */

  uint8_t dma_enable;              /*!< USB DMA state.
                                         If DMA is not supported this parameter shall be set by default to zero */

  uint8_t speed;                   /*!< USB Core speed.
                                        This parameter can be any value of @ref PCD_Speed/HCD_Speed
                                                                                (HCD_SPEED_xxx, HCD_SPEED_xxx) */

  uint8_t ep0_mps;                 /*!< Set the Endpoint 0 Max Packet size.                                    */

  uint8_t phy_itface;              /*!< Select the used PHY interface.
                                        This parameter can be any value of @ref PCD_PHY_Module/HCD_PHY_Module  */

  uint8_t Sof_enable;              /*!< Enable or disable the output of the SOF signal.                        */

  uint8_t low_power_enable;        /*!< Enable or disable the low Power Mode.                                  */

  uint8_t lpm_enable;              /*!< Enable or disable Link Power Management.                               */

  uint8_t battery_charging_enable; /*!< Enable or disable Battery charging.                                    */

  uint8_t vbus_sensing_enable;     /*!< Enable or disable the VBUS Sensing feature.                            */

#if defined (USB_OTG_FS) || defined (USB_OTG_HS)
  uint8_t use_dedicated_ep1;       /*!< Enable or disable the use of the dedicated EP1 interrupt.              */

  uint8_t use_external_vbus;       /*!< Enable or disable the use of the external VBUS.                        */
#endif /* defined (USB_OTG_FS) || defined (USB_OTG_HS) */
#if defined (USB_DRD_FS)
  uint8_t bulk_doublebuffer_enable;  /*!< Enable or disable the double buffer mode on bulk EP                  */

  uint8_t iso_singlebuffer_enable;   /*!< Enable or disable the Single buffer mode on Isochronous  EP          */
#endif /* defined (USB_DRD_FS) */
} USB_CfgTypeDef;

typedef struct
{
  uint8_t   num;                  /*!< Endpoint number
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 15   */

  uint8_t   is_in;                /*!< Endpoint direction
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

  uint8_t   is_stall;             /*!< Endpoint stall condition
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

#if defined (USB_OTG_FS) || defined (USB_OTG_HS)
  uint8_t   is_iso_incomplete;    /*!< Endpoint isoc condition
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */
#endif /* defined (USB_OTG_FS) || defined (USB_OTG_HS) */

  uint8_t   type;                 /*!< Endpoint type
                                       This parameter can be any value of @ref USB_LL_EP_Type                   */

  uint8_t   data_pid_start;       /*!< Initial data PID
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

#if defined (USB_DRD_FS)
  uint16_t  pmaadress;            /*!< PMA Address
                                       This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */

  uint16_t  pmaaddr0;             /*!< PMA Address0
                                       This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */

  uint16_t  pmaaddr1;             /*!< PMA Address1
                                       This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */

  uint8_t   doublebuffer;         /*!< Double buffer enable
                                       This parameter can be 0 or 1                                             */
#endif /* defined (USB_DRD_FS) */

  uint32_t  maxpacket;            /*!< Endpoint Max packet size
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 64KB */

  uint8_t   *xfer_buff;           /*!< Pointer to transfer buffer                                               */

  uint32_t  xfer_len;             /*!< Current transfer length                                                  */

  uint32_t  xfer_count;           /*!< Partial transfer length in case of multi packet transfer                 */

#if defined (USB_OTG_FS) || defined (USB_OTG_HS)
  uint8_t   even_odd_frame;       /*!< IFrame parity
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

  uint16_t  tx_fifo_num;          /*!< Transmission FIFO number
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 15   */

  uint32_t  dma_addr;             /*!< 32 bits aligned transfer buffer address                                  */

  uint32_t  xfer_size;            /*!< requested transfer size                                                  */
#endif /* defined (USB_OTG_FS) || defined (USB_OTG_HS) */

#if defined (USB_DRD_FS)
  uint32_t  xfer_len_db;          /*!< double buffer transfer length used with bulk double buffer in            */

  uint8_t   xfer_fill_db;         /*!< double buffer Need to Fill new buffer  used with bulk_in                 */
#endif /* defined (USB_DRD_FS) */
} USB_EPTypeDef;



#if defined (USB_DRD_FS)
typedef USB_EPTypeDef       USB_DRD_EPTypeDef;
#endif /* defined (USB_DRD_FS) */

/* Exported constants --------------------------------------------------------*/

/** @defgroup USB_LL_EP_Type USB Low Layer EP Type
  * @{
  */
#define EP_TYPE_CTRL                           0U
#define EP_TYPE_ISOC                           1U
#define EP_TYPE_BULK                           2U
#define EP_TYPE_INTR                           3U
#define EP_TYPE_MSK                            3U


#if defined (USB_DRD_FS)

/* Exported macro ------------------------------------------------------------*/
/**
  * @}
  */
/********************  Bit definition for USB_COUNTn_RX register  *************/
#define USB_CNTRX_NBLK_MSK                    (0x1FU << 26)
#define USB_CNTRX_BLSIZE                      (0x1U << 31)


/*Set Channel/Endpoint to the USB Register */
#define USB_DRD_SET_CHEP(USBx, bEpChNum, wRegValue)  (*(__IO uint32_t *)\
                                                      (&(USBx)->CHEP0R + (bEpChNum)) = (uint32_t)(wRegValue))

/*Get Channel/Endpoint from the USB Register */
#define USB_DRD_GET_CHEP(USBx, bEpChNum)             (*(__IO uint32_t *)(&(USBx)->CHEP0R + (bEpChNum)))


/**
  * @brief  sets the status for tx transfer (bits STAT_TX[1:0]).
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @param  wState new state
  * @retval None
  */
#define USB_DRD_SET_CHEP_TX_STATUS(USBx, bEpChNum, wState) \
  do { \
    uint32_t _wRegVal; \
    \
    _wRegVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_CHEP_TX_DTOGMASK; \
    /* toggle first bit ? */ \
    if ((USB_CHEP_TX_DTOG1 & (wState)) != 0U) \
    { \
      _wRegVal ^= USB_CHEP_TX_DTOG1; \
    } \
    /* toggle second bit ?  */ \
    if ((USB_CHEP_TX_DTOG2 & (wState)) != 0U) \
    { \
      _wRegVal ^= USB_CHEP_TX_DTOG2; \
    } \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wRegVal | USB_CHEP_VTRX| USB_CHEP_VTTX)); \
  } while(0) /* USB_DRD_SET_CHEP_TX_STATUS */


/**
  * @brief  sets the status for rx transfer (bits STAT_TX[1:0])
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @param  wState new state
  * @retval None
  */
#define USB_DRD_SET_CHEP_RX_STATUS(USBx, bEpChNum, wState) \
  do { \
    uint32_t _wRegVal; \
    \
    _wRegVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_CHEP_RX_DTOGMASK; \
    /* toggle first bit ? */ \
    if ((USB_CHEP_RX_DTOG1 & (wState)) != 0U) \
    { \
      _wRegVal ^= USB_CHEP_RX_DTOG1; \
    } \
    /* toggle second bit ? */ \
    if ((USB_CHEP_RX_DTOG2 & (wState)) != 0U) \
    { \
      _wRegVal ^= USB_CHEP_RX_DTOG2; \
    } \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wRegVal | USB_CHEP_VTRX | USB_CHEP_VTTX)); \
  } while(0) /* USB_DRD_SET_CHEP_RX_STATUS */


/**
  * @brief  gets the status for tx/rx transfer (bits STAT_TX[1:0]
  *         /STAT_RX[1:0])
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @retval status
  */
#define USB_DRD_GET_CHEP_TX_STATUS(USBx, bEpChNum) \
  ((uint16_t)USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_DRD_CHEP_TX_STTX)

#define USB_DRD_GET_CHEP_RX_STATUS(USBx, bEpChNum) \
  ((uint16_t)USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_DRD_CHEP_RX_STRX)


/**
  * @brief  set  EP_KIND bit.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @retval None
  */
#define USB_DRD_SET_CHEP_KIND(USBx, bEpChNum) \
  do { \
    uint32_t _wRegVal; \
    \
    _wRegVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_CHEP_REG_MASK; \
    \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wRegVal | USB_CHEP_VTRX | USB_CHEP_VTTX | USB_CHEP_KIND)); \
  } while(0) /* USB_DRD_SET_CHEP_KIND */


/**
  * @brief  clear EP_KIND bit.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @retval None
  */
#define USB_DRD_CLEAR_CHEP_KIND(USBx, bEpChNum) \
  do { \
    uint32_t _wRegVal; \
    \
    _wRegVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_EP_KIND_MASK; \
    \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wRegVal | USB_CHEP_VTRX | USB_CHEP_VTTX)); \
  } while(0) /* USB_DRD_CLEAR_CHEP_KIND */


/**
  * @brief  Clears bit CTR_RX / CTR_TX in the endpoint register.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @retval None
  */
#define USB_DRD_CLEAR_RX_CHEP_CTR(USBx, bEpChNum) \
  do { \
    uint32_t _wRegVal; \
    \
    _wRegVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)) & (0xFFFF7FFFU & USB_CHEP_REG_MASK); \
    \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wRegVal | USB_CHEP_VTTX)); \
  } while(0) /* USB_CLEAR_RX_CHEP_CTR */

#define USB_DRD_CLEAR_TX_CHEP_CTR(USBx, bEpChNum) \
  do { \
    uint32_t _wRegVal; \
    \
    _wRegVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)) & (0xFFFFFF7FU & USB_CHEP_REG_MASK); \
    \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wRegVal | USB_CHEP_VTRX)); \
  } while(0) /* USB_CLEAR_TX_CHEP_CTR */


/**
  * @brief  Toggles DTOG_RX / DTOG_TX bit in the endpoint register.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @retval None
  */
#define USB_DRD_RX_DTOG(USBx, bEpChNum) \
  do { \
    uint32_t _wEPVal; \
    \
    _wEPVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_CHEP_REG_MASK; \
    \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wEPVal | USB_CHEP_VTRX | USB_CHEP_VTTX | USB_CHEP_DTOG_RX)); \
  } while(0) /* USB_DRD_RX_DTOG */

#define USB_DRD_TX_DTOG(USBx, bEpChNum) \
  do { \
    uint32_t _wEPVal; \
    \
    _wEPVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_CHEP_REG_MASK; \
    \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wEPVal | USB_CHEP_VTRX | USB_CHEP_VTTX | USB_CHEP_DTOG_TX)); \
  } while(0) /* USB_TX_DTOG */


/**
  * @brief  Clears DTOG_RX / DTOG_TX bit in the endpoint register.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @retval None
  */
#define USB_DRD_CLEAR_RX_DTOG(USBx, bEpChNum) \
  do { \
    uint32_t _wRegVal; \
    \
    _wRegVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)); \
    \
    if ((_wRegVal & USB_CHEP_DTOG_RX) != 0U) \
    { \
      USB_DRD_RX_DTOG((USBx), (bEpChNum)); \
    } \
  } while(0) /* USB_DRD_CLEAR_RX_DTOG */

#define USB_DRD_CLEAR_TX_DTOG(USBx, bEpChNum) \
  do { \
    uint32_t _wRegVal; \
    \
    _wRegVal = USB_DRD_GET_CHEP((USBx), (bEpChNum)); \
    \
    if ((_wRegVal & USB_CHEP_DTOG_TX) != 0U) \
    { \
      USB_DRD_TX_DTOG((USBx), (bEpChNum)); \
    } \
  } while(0) /* USB_DRD_CLEAR_TX_DTOG */


/**
  * @brief  Sets address in an endpoint register.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @param  bAddr Address.
  * @retval None
  */
#define USB_DRD_SET_CHEP_ADDRESS(USBx, bEpChNum, bAddr) \
  do { \
    uint32_t _wRegVal; \
    \
    /*Read the USB->CHEPx into _wRegVal, Reset(DTOGRX/STRX/DTOGTX/STTX) and set the EpAddress*/ \
    _wRegVal = (USB_DRD_GET_CHEP((USBx), (bEpChNum)) & USB_CHEP_REG_MASK) | (bAddr); \
    \
    /*Set _wRegVal in USB->CHEPx and set Transmit/Receive Valid Transfer  (x=bEpChNum)*/ \
    USB_DRD_SET_CHEP((USBx), (bEpChNum), (_wRegVal | USB_CHEP_VTRX | USB_CHEP_VTTX)); \
  } while(0) /* USB_DRD_SET_CHEP_ADDRESS */


/* PMA API Buffer Descriptor Management ------------------------------------------------------------*/
/* Buffer Descriptor Table   TXBD0/RXBD0 --- > TXBD7/RXBD7  8 possible descriptor
* The buffer descriptor is located inside the packet buffer memory (USB_PMA_BUFF)
*          TXBD    [Reserve         |Countx| Address_Tx]
*          RXBD    [BLSIEZ|NUM_Block |CounRx| Address_Rx] */

/* Set TX Buffer Descriptor Address Field */
#define USB_DRD_SET_CHEP_TX_ADDRESS(USBx, bEpChNum, wAddr) \
  do { \
    /* Reset old Address */ \
    (USB_DRD_PMA_BUFF + (bEpChNum))->TXBD &= USB_PMA_TXBD_ADDMSK; \
    \
    /* Bit0 & Bit1 should be =0 PMA must be Word aligned */ \
    (USB_DRD_PMA_BUFF + (bEpChNum))->TXBD |= (uint32_t)(((uint32_t)(wAddr) >> 2U) << 2U); \
  } while(0) /* USB_DRD_SET_CHEP_TX_ADDRESS */

/* Set RX Buffer Descriptor Address Field */
#define USB_DRD_SET_CHEP_RX_ADDRESS(USBx, bEpChNum, wAddr) \
  do { \
    /* Reset old Address */ \
    (USB_DRD_PMA_BUFF + (bEpChNum))->RXBD &= USB_PMA_RXBD_ADDMSK; \
    \
    /* Bit0 & Bit1 should be =0 PMA must be Word aligned */ \
    (USB_DRD_PMA_BUFF + (bEpChNum))->RXBD |= (uint32_t)(((uint32_t)(wAddr) >> 2U) << 2U); \
  } while(0) /* USB_SET_CHEP_RX_ADDRESS */


/**
  * @brief  Sets counter of rx buffer with no. of blocks.
  * @param  pdwReg Register pointer
  * @param  wCount Counter.
  * @param  wNBlocks no. of Blocks.
  * @retval None
  */
#define USB_DRD_CALC_BLK32(pdwReg, wCount, wNBlocks) \
  do { \
    /* Divide PacketSize by 32 to calculate the Nb of Block32 */ \
    (wNBlocks) =((uint32_t)(wCount) >> 5U); \
    if (((uint32_t)(wCount) % 32U) == 0U)  \
    { \
      (wNBlocks)--; \
    } \
    \
    (pdwReg)|= (uint32_t)((((wNBlocks) << 26U)) | USB_CNTRX_BLSIZE); \
  } while(0) /* USB_DRD_CALC_BLK32 */

#define USB_DRD_CALC_BLK2(pdwReg, wCount, wNBlocks) \
  do { \
    /* Divide PacketSize by 32 to calculate the Nb of Block32 */ \
    (wNBlocks) = (uint32_t)((uint32_t)(wCount) >> 1U); \
    if (((wCount) & 0x1U) != 0U) \
    { \
      (wNBlocks)++; \
    } \
    (pdwReg) |= (uint32_t)((wNBlocks) << 26U); \
  } while(0) /* USB_DRD_CALC_BLK2 */

#define USB_DRD_SET_CHEP_CNT_RX_REG(pdwReg, wCount) \
  do { \
    uint32_t wNBlocks; \
    \
    (pdwReg) &= ~(USB_CNTRX_BLSIZE | USB_CNTRX_NBLK_MSK); \
    \
    if ((wCount) == 0U) \
    { \
      (pdwReg) |= USB_CNTRX_BLSIZE; \
    } \
    else if ((wCount) <= 62U) \
    { \
      USB_DRD_CALC_BLK2((pdwReg), (wCount), wNBlocks); \
    } \
    else \
    { \
      USB_DRD_CALC_BLK32((pdwReg), (wCount), wNBlocks); \
    } \
  } while(0) /* USB_DRD_SET_CHEP_CNT_RX_REG */


/**
  * @brief  sets counter for the tx/rx buffer.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @param  wCount Counter value.
  * @retval None
  */
#define USB_DRD_SET_CHEP_TX_CNT(USBx,bEpChNum, wCount) \
  do { \
    /* Reset old TX_Count value */ \
    (USB_DRD_PMA_BUFF + (bEpChNum))->TXBD &= USB_PMA_TXBD_COUNTMSK; \
    \
    /* Set the wCount in the dedicated EP_TXBuffer */ \
    (USB_DRD_PMA_BUFF + (bEpChNum))->TXBD |= (uint32_t)((uint32_t)(wCount) << 16U); \
  } while(0)

/*#define USB_DRD_SET_CHEP_RX_DBUF0_CNT(USBx, bEpChNum, wCount) \
  USB_DRD_SET_CHEP_CNT_RX_REG(((USB_DRD_PMA_BUFF + (bEpChNum))->TXBD), (wCount))
*/
#define USB_DRD_SET_CHEP_RX_CNT(USBx, bEpChNum, wCount) \
  USB_DRD_SET_CHEP_CNT_RX_REG(((USB_DRD_PMA_BUFF + (bEpChNum))->RXBD), (wCount))

/**
  * @brief  gets counter of the tx buffer.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @retval Counter value
  */
#define USB_DRD_GET_CHEP_TX_CNT(USBx, bEpChNum)           (((USB_DRD_PMA_BUFF + (bEpChNum))->TXBD & 0x03FF0000U) >> 16U)
#define USB_DRD_GET_CHEP_RX_CNT(USBx, bEpChNum)           (((USB_DRD_PMA_BUFF + (bEpChNum))->RXBD & 0x03FF0000U) >> 16U)

/**
  * @brief  Gets buffer 0/1 address of a double buffer endpoint.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @param  bDir endpoint dir  EP_DBUF_OUT = OUT
  *         EP_DBUF_IN  = IN
  * @param  wCount: Counter value
  * @retval None
  */
#define USB_DRD_SET_CHEP_DBUF0_CNT(USBx, bEpChNum, bDir, wCount) \
  do { \
    if ((bDir) == 0U) \
    { \
      /* OUT endpoint */ \
      USB_DRD_SET_CHEP_RX_DBUF0_CNT((USBx), (bEpChNum), (wCount)); \
    } \
    else \
    { \
      if ((bDir) == 1U) \
      { \
        /* IN endpoint */ \
        USB_DRD_SET_CHEP_TX_CNT((USBx), (bEpChNum), (wCount)); \
      } \
    } \
  } while(0) /* USB_DRD_SET_CHEP_DBUF0_CNT */

#define USB_DRD_SET_CHEP_DBUF1_CNT(USBx, bEpChNum, bDir, wCount) \
  do { \
    if ((bDir) == 0U) \
    { \
      /* OUT endpoint */ \
      USB_DRD_SET_CHEP_RX_CNT((USBx), (bEpChNum), (wCount)); \
    } \
    else \
    { \
      if ((bDir) == 1U) \
      { \
        /* IN endpoint */ \
        (USB_DRD_PMA_BUFF + (bEpChNum))->RXBD &= USB_PMA_TXBD_COUNTMSK; \
        (USB_DRD_PMA_BUFF + (bEpChNum))->RXBD |= (uint32_t)((uint32_t)(wCount) << 16U); \
      } \
    } \
  } while(0) /* USB_DRD_SET_CHEP_DBUF1_CNT */


/**
  * @brief  Gets buffer 0/1 rx/tx counter for double buffering.
  * @param  USBx USB peripheral instance register address.
  * @param  bEpChNum Endpoint Number.
  * @retval None
  */
#define USB_DRD_GET_CHEP_DBUF0_CNT(USBx, bEpChNum)     (USB_DRD_GET_CHEP_TX_CNT((USBx), (bEpChNum)))
#define USB_DRD_GET_CHEP_DBUF1_CNT(USBx, bEpChNum)     (USB_DRD_GET_CHEP_RX_CNT((USBx), (bEpChNum)))
#endif /* defined (USB_DRD_FS) */

void USB_ll_init();
int USB_ActivateEndpoint(USB_DRD_TypeDef *USBx, USB_DRD_EPTypeDef *ep);
void USB_WritePMA(USB_DRD_TypeDef const *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes);
void USB_ReadPMA(USB_DRD_TypeDef const *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes);

#endif /* INC_USB_LL_H_ */
