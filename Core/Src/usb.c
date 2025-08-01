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

#include "usb.h"
#include "usb_descriptors.h"
#include "cli.h"
#include "stm32h5xx_ll_utils.h"
#include "gpio.h"
#include <string.h>

#define NUM_BUFF_DESCR_ENTRY 	8

#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define RESERVED1			2
#define SET_FEATURE			3
#define RESERVED2			4
#define SET_ADDRESS			5
#define GET_DESCRIPTOR		6
#define SET_DESCRIPTOR		7
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11
#define SYNCH_FRAME			12

static uint8_t received_dev_address;
static uint8_t change_address;
static int8_t configuration_num = 0;

/*
 * Assuming that for each IN endpoint there is a corresponding OUT endpoint,
 * and considering that each element of ep_remaining_bytes[] and ep_data_p[] arrays
 * serves a pair of IN and OUT endpoints, their length is TOT_ENDPOINT_COUNT/2
 */
static int ep_remaining_bytes[TOT_ENDPOINT_COUNT/2];
static uint8_t* ep_data_p[TOT_ENDPOINT_COUNT/2];
static uint8_t ep_state[TOT_ENDPOINT_COUNT/2];

enum usb_dev_state {
	USB_NONE,
	USB_ATTACHED,
	USB_POWERED,
	USB_DEFAULT,
	USB_ADDRESS,
	USB_CONFIGURED,
	USB_SUSPENDED
};

enum  {
	SETUP,
	DATA_OUT,
	DATA_IN,
	STATUS_IN,
	STATUS_OUT
};// ep0_state;

enum {
	EP_REQ,
	EP_OUT,
	EP_IN
};

static enum usb_dev_state dev_state = USB_NONE;

USB_DRD_EPTypeDef ch_ep_in[NUM_BUFF_DESCR_ENTRY];
USB_DRD_EPTypeDef ch_ep_out[NUM_BUFF_DESCR_ENTRY];

static void error(const char* str, int err_no)
{
	STRPRINT("\n*** ERROR %s (%d) ****\n",str,err_no);
}

void usb_reset_isr()
{
	uint32_t packet_buffer_start = ((2+BULK_ENDPOINT_COUNT) * 4);	// one 32-bit USBRAM register is needed for each endpoint before the packet buffers

	STRPRINT("USB Resetting..\n");

	/* Set up structs for default endpoints 0 IN and OUT */
	ch_ep_out[0].num=0;
	ch_ep_out[0].pmaadress = packet_buffer_start;
	ch_ep_out[0].maxpacket=EP_MAX_PACKET_SIZE;
	ch_ep_out[0].is_in=0;
	ch_ep_out[0].type=EP_TYPE_CTRL;
	USB_ActivateEndpoint(USB_DRD_FS, &ch_ep_out[0]);
	packet_buffer_start += EP_MAX_PACKET_SIZE;

	ch_ep_in[0].num=0;
	ch_ep_in[0].pmaadress=packet_buffer_start;
	ch_ep_in[0].maxpacket=EP_MAX_PACKET_SIZE;
	ch_ep_in[0].is_in=1;
	ch_ep_in[0].type=EP_TYPE_CTRL;
	USB_ActivateEndpoint(USB_DRD_FS, &ch_ep_in[0]);
	packet_buffer_start += EP_MAX_PACKET_SIZE;

	/* Set up structs for BULK endpoints */
	for(int i=1;i<BULK_ENDPOINT_COUNT/2+1;i++) {
		ch_ep_out[i].num=i;
		ch_ep_out[i].pmaadress=packet_buffer_start;
		ch_ep_out[i].maxpacket=EP_MAX_PACKET_SIZE;
		ch_ep_out[i].is_in=0;
		ch_ep_out[i].type=EP_TYPE_BULK;
		ch_ep_out[i].data_pid_start=0;
		ch_ep_out[i].is_stall = 0;
		USB_ActivateEndpoint(USB_DRD_FS, &ch_ep_out[i]);
		packet_buffer_start += EP_MAX_PACKET_SIZE;

		ch_ep_in[i].num=i;
		ch_ep_in[i].pmaadress=packet_buffer_start;
		ch_ep_in[i].maxpacket=EP_MAX_PACKET_SIZE;
		ch_ep_in[i].is_in=1;
		ch_ep_in[i].type=EP_TYPE_BULK;
		ch_ep_in[i].data_pid_start=0;
		ch_ep_in[i].is_stall = 1;
		USB_ActivateEndpoint(USB_DRD_FS, &ch_ep_in[i]);
		packet_buffer_start += EP_MAX_PACKET_SIZE;
	}

	configuration_num = 0;
	received_dev_address = 0;
	change_address = 0;
	USB_DRD_FS->DADDR = 0x80 | received_dev_address;

	dev_state = USB_DEFAULT;
	ep_state[0] = SETUP;
	for(int i=1;i<TOT_ENDPOINT_COUNT/2;i++)
	{
		ep_state[i] = EP_REQ;
	}

	USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_NAK);
	USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,1,USB_EP_TX_NAK);
	USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
	USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,1,USB_EP_RX_VALID);
}


static int min(int v1, int v2)
{
	return (v1<v2 ? v1 : v2);
}

static void stall_ep(int ep_num)
{
	USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,ep_num,USB_EP_RX_STALL);
	USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,ep_num,USB_EP_TX_STALL);
}

static void reset_ep(int ep_num)
{
	ep_remaining_bytes[ep_num] = 0;
	ep_state[ep_num] = EP_REQ;
	USB_DRD_CLEAR_RX_DTOG(USB_DRD_FS,ep_num);
	USB_DRD_CLEAR_TX_DTOG(USB_DRD_FS,ep_num);
	USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,ep_num,USB_EP_RX_VALID);
	USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,ep_num,USB_EP_TX_NAK);
}

static int prepare_descriptor(const struct usb_request* request)
{
	uint8_t descriptor_type = request->wValue >> 8;
	uint8_t index = request->wValue & 0xFF;

	switch(descriptor_type) {

	case DESCR_DEVICE:
		ep_remaining_bytes[0] = min(request->wLength,sizeof(usb_device_desc));
		ep_data_p[0] = (uint8_t*)&usb_device_desc;
		return 0;

	case DESCR_CONFIGURATION:
		ep_remaining_bytes[0] = min(request->wLength,usb_framework_desc.configuration.wTotalLength);
		ep_data_p[0] = (uint8_t*)&usb_framework_desc;
		return 0;

	case DESCR_STRING:
		// search the array of usb strings for the string index
		for(int i=0;usb_strings[i].index!=0xFF;i++) {
			if(usb_strings[i].index == index) {
				ep_remaining_bytes[0] = min(request->wLength,usb_strings[i].ptr[0]);
				ep_data_p[0] = (uint8_t*)usb_strings[i].ptr;
				return 0;
			}
		}
		STRPRINT("\nString id=%d not supported\n",index);
		return -1;

	case DESCR_BOS:
		ep_remaining_bytes[0] = min(request->wLength,sizeof(bosDescriptor));
		ep_data_p[0] = (uint8_t*)&bosDescriptor;
		return 0;

	case DESCR_INTERFACE:
		ep_remaining_bytes[0] = min(request->wLength,sizeof(usb_framework_desc.interface));
		ep_data_p[0] = (uint8_t*)&(usb_framework_desc.interface);
		return 0;

	case DESCR_ENDPOINT:
		ep_remaining_bytes[0] = min(request->wLength,sizeof(struct usb_endpoint_descriptor));
		if((request->wValue & 0x80) == 0)
			ep_data_p[0] = (uint8_t*)&(usb_framework_desc.endpoints[0]);//endpoint1_out);
		else
			ep_data_p[0] = (uint8_t*)&(usb_framework_desc.endpoints[1]);//endpoint1_in);
		return 0;

	case DESCR_DEVICE_QUALIFER:
		/*
		 * The device_qualifier descriptor describes information about a high-speed capable device that would
		 * change if the device were operating at the other speed.
		 * If a full-speed only device (with a device descriptor version number equal to 0200H) receives a
		 * GetDescriptor() request for a device_qualifier, it must respond with a request error
		 */
	case DESCR_OTHER_SPEED_CONFIGURATION:
	case DESCR_INTERFACE_POWER:
	default:
		USB_DRD_SET_CHEP_TX_CNT(USB_DRD_FS,0,0);
		STRPRINT("Descriptor=%d not supported\n",descriptor_type);
		return -2;
	}

}

static int prepare_os_descriptor(const struct usb_request* request)
{
	if(request->bmRequestType ==0xC0 && request->wIndex==7 && request->bRequest==REQUEST_GET_MS_DESCRIPTOR) {

		ep_remaining_bytes[0] = min(request->wLength, sizeof(msOs20DescriptorSet));
		ep_data_p[0] = (uint8_t*)&msOs20DescriptorSet;
		return 0;
	}
	else if(request->bmRequestType ==0xC0 && request->wIndex==4 && request->bRequest==usb_OS_string_desc.bMS_VendorCode) {

		ep_remaining_bytes[0] = min(request->wLength, sizeof(usb_ms_compatible_ID_feature_desc));
		ep_data_p[0] = (uint8_t*)&usb_ms_compatible_ID_feature_desc;
		return 0;
	}
	else if(request->bmRequestType ==0xC1 && request->wIndex==5 && request->bRequest==usb_OS_string_desc.bMS_VendorCode) {

		ep_remaining_bytes[0] = min(request->wLength, sizeof(usb_extended_property_feature_desc));
		ep_data_p[0] = (uint8_t*)&usb_extended_property_feature_desc;
		return 0;
	}
	return -1;
}

void ep0_ready_tx_packet(uint8_t* data, uint32_t bytes_to_send)
{
	uint32_t xfer_count = min(bytes_to_send,EP_MAX_PACKET_SIZE);
	USB_WritePMA(USB_DRD_FS,data,((USB_DRD_PMA_BUFF)->TXBD) & 0xFF, xfer_count);
	USB_DRD_SET_CHEP_TX_CNT(USB_DRD_FS,0,xfer_count);
	ep_state[0] = DATA_IN;
	/*
	 * A USB Device is required to STALL the transaction in the case of errors. To do so, at all data stages before the last,
	 * the unused direction must be set to STALL, so that, if the host reverses the transfer direction too soon,
	 * it gets a STALL as a status stage.
	 *
	 * Before enabling the last data transaction, the opposite direction must be set to NAK
	 * so that, if the host reverses the transaction direction (to perform the status stage)
	 * too soon, it is kept waiting for the completion of the data stage.
	 * If the control operation completes successfully, the software changes NAK to VALID, otherwise to STALL.
	 */
	if(bytes_to_send <= EP_MAX_PACKET_SIZE)
		USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_NAK);
	else
		USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_STALL);
	USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_VALID);
}
/*
Control transfers are made of a SETUP transaction, followed by zero or more data stages,
all of the same direction, followed by a status stage (a zero-byte transfer in the opposite
direction). SETUP transactions are handled by control endpoints only and are very similar to
OUT ones (data reception) except that the values of DTOGTX and DTOGRX bits of the
addressed endpoint registers are set to 1 and 0 respectively, to initialize the control transfer,
and both STATTX and STATRX are set to 10 (NAK) to let software decide if subsequent
transactions must be IN or OUT depending on the SETUP contents. A control endpoint must
check SETUP bit in the USB_CHEPnR register at each VTRX event to distinguish normal
OUT transactions from SETUP ones. A USB Device can determine the number and
direction of data stages by interpreting the data transferred in the SETUP stage, and is
required to STALL the transaction in the case of errors. To do so, at all data stages before
the last, the unused direction must be set to STALL, so that, if the host reverses the transfer
direction too soon, it gets a STALL as a status stage.

While enabling the last data stage, the opposite direction must be set to NAK, so that, if the
host reverses the transfer direction (to perform the status stage) immediately, it is kept
waiting for the completion of the control operation. If the control operation completes
successfully, the software changes NAK to VALID, otherwise to STALL. At the same time, if
the status stage is an OUT, the STATUS_OUT (EPKIND in the USB_CHEPnR register) bit
must be set, so that an error is generated if a status transaction is performed with non-zero
data. When the status transaction is serviced, the application clears the STATUS_OUT bit
and sets STATRX to VALID (to accept a new command) and STATTX to NAK (to delay a
possible status stage immediately following the next setup).

Since the USB specification states that a SETUP packet cannot be answered with a
handshake different from ACK, eventually aborting a previously issued command to start
the new one, the USB logic does not permit a control endpoint to answer with a NAK or
STALL packet to a SETUP token received from the host.

When the STATRX bits are set to 01 (STALL) or 10 (NAK) and a SETUP token is received,
the USB accepts the data, performing the required data transfers and sends back an ACK
handshake. If that endpoint has a previously issued VTRX request not yet acknowledged by
the application (for example VTRX bit is still set from a previously completed reception), the
USB discards the SETUP transaction and does not answer with any handshake packet
regardless of its state, simulating a reception error and forcing the host to send the SETUP
token again. This is done to avoid losing the notification of a SETUP transaction addressed
to the same endpoint immediately following the transaction, which triggered the VTRX
interrupt.
 */
int ep0_sm(uint32_t istr)
{
	uint32_t xfer_count;
	struct usb_request data;

	uint16_t ch_ep = (uint16_t)USB_DRD_GET_CHEP(USB_DRD_FS,0);

	switch(ep_state[0]) {

	case SETUP:
		if( (ch_ep & USB_EP_SETUP) == 0 || (istr & USB_ISTR_DIR) == 0)
			error(__FUNCTION__,-1);

		USB_DRD_CLEAR_RX_CHEP_CTR(USB_DRD_FS, 0);

		/*
		 * Once a SETUP packet has been received, set tx status = NAK to recover from a possibly previous STALL status,
		 * so that if the host sends a DATA IN token, the device does not replies with STALL packet, but with a NAK packet to buy some time.
		 * The tx status will be updated after examining the request.
		 */
		USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_NAK);

		ep_remaining_bytes[0]=0;
		ep_data_p[0]=NULL;
		STRPRINT(" SETUP, count_rx=%d, dev_addr=0x%02X\n",USB_DRD_GET_CHEP_RX_CNT(USB_DRD_FS,0), USB_DRD_FS->DADDR & 0x7F);
		xfer_count = (uint16_t)USB_DRD_GET_CHEP_RX_CNT(USB_DRD_FS, 0);
		USB_ReadPMA(USB_DRD_FS, (uint8_t *)&data,((USB_DRD_PMA_BUFF)->RXBD) & 0xFF, (uint16_t)xfer_count);
		STRPRINT("\tbmRequestType: 0x%2X\n\tbRequest: %d\n\twValue: 0x%02X\n\twIndex: %d\n\twLength: %d\n",data.bmRequestType,data.bRequest,data.wValue,data.wIndex,data.wLength);

		// First check if the request is for a Microsoft OS Feature Descriptor
		if((data.bmRequestType & 0xFE)==0xC0) {
			if(prepare_os_descriptor(&data)>=0) {
				ep0_ready_tx_packet(ep_data_p[0], ep_remaining_bytes[0]);
			}
			else {
				STRPRINT("bRequest=%d is not supported\n",data.bRequest);
				ep_state[0] = SETUP;
				USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_STALL);
				USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
			}
		}
		// then evaluate standard requests
		else {
			switch(data.bRequest) {

			case CLEAR_FEATURE:
				if(data.bmRequestType==0x2 && data.wValue==0) // Feature selector == 0 -> clear ENDPOINT_HALT
					reset_ep(data.wIndex);
				USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
				break;

			case SET_ADDRESS:
				/*
				 * Store the received address in a global variable so that the device address
				 * can be changed once the request has completed successfully, i.e., after the status transaction
				 */
				received_dev_address = (uint8_t)(data.wValue);
				change_address = 1;
				USB_DRD_SET_CHEP_TX_CNT(USB_DRD_FS,0,0);
				ep_state[0] = STATUS_IN;
				USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_VALID);
				break;

			case GET_DESCRIPTOR:
				int ret = prepare_descriptor(&data);
				if( ret < 0) {	// descriptor not supported -> return a STALL PID in the next DATA transaction
					ep_state[0] = SETUP;
					ep_remaining_bytes[0] = 0;
					USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_STALL);
					USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
				}
				else {
					ep0_ready_tx_packet(ep_data_p[0], ep_remaining_bytes[0]);
				}
				break;

			case SET_CONFIGURATION:
				if(dev_state < USB_ADDRESS ) {
					ep_state[0] = SETUP;
					USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_STALL);
					USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
				}
				else {
					if (data.wValue==1) {
						configuration_num = 1;
						dev_state = USB_CONFIGURED;
					}
					else {
						configuration_num = 0;
						dev_state = USB_ADDRESS;
					}
					USB_DRD_SET_CHEP_TX_CNT(USB_DRD_FS,0,0);
					ep_state[0] = STATUS_IN;
					USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_VALID);
					// make end point 1 and 2 ready to receive
					USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,1,USB_EP_RX_VALID);
					USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,2,USB_EP_RX_VALID);
				}
				break;

			case GET_STATUS:
				uint8_t status[2] = {0x01,0x00};
				ep0_ready_tx_packet(status, sizeof(status));
				break;

			case GET_CONFIGURATION:
				uint8_t num = configuration_num;;
				if(dev_state < USB_CONFIGURED)
					num = 0;
				ep0_ready_tx_packet(&num, sizeof(num));
				break;

			case SET_FEATURE:
			case SET_DESCRIPTOR:
			case GET_INTERFACE:
			case SET_INTERFACE:
			case SYNCH_FRAME:
			default:
				STRPRINT("bRequest=%d is not supported\n",data.bRequest);
				ep_state[0] = SETUP;
				USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_STALL);
				USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
				break;
			}
		}
		break;

	case DATA_IN:

		if( (ch_ep & USB_EP_SETUP) != 0 || (istr & USB_ISTR_DIR) != 0 )
			error(__FUNCTION__,-5);
		USB_DRD_CLEAR_TX_CHEP_CTR(USB_DRD_FS, 0);

		xfer_count = USB_DRD_GET_CHEP_TX_CNT(USB_DRD_FS,0);
		if(xfer_count == EP_MAX_PACKET_SIZE) {
			ep_remaining_bytes[0] -= xfer_count;
			ep_data_p[0] += xfer_count;
			xfer_count = min(ep_remaining_bytes[0],EP_MAX_PACKET_SIZE);
			USB_WritePMA(USB_DRD_FS,ep_data_p[0],((USB_DRD_PMA_BUFF)->TXBD) & 0xFF, xfer_count);
			USB_DRD_SET_CHEP_TX_CNT(USB_DRD_FS,0,xfer_count);
			/*
			 * A USB Device is required to STALL the transaction in the case of errors. To do so, at all data stages before the last,
			 * the unused direction must be set to STALL, so that, if the host reverses the transfer direction too soon,
			 * it gets a STALL as a status stage.
			 *
			 * Before enabling the last data transaction, the opposite direction must be set to NAK
			 * so that, if the host reverses the transaction direction (to perform the status stage)
			 * too soon, it is kept waiting for the completion of the data stage.
			 * If the control operation completes successfully, the software changes NAK to VALID, otherwise to STALL.
			 */
			if(ep_remaining_bytes[0] <= EP_MAX_PACKET_SIZE)
				USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_NAK);
			else
				USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_STALL);
			USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_VALID);
		}
		else {
			/* The STATUS_OUT (EPKIND in the USB_CHEP0R) bit must be set, so that an error is generated if a status
			 * transaction is performed with non-zero data
			 */
			USB_DRD_SET_CHEP_KIND(USB_DRD_FS,0);
			ep_state[0] = STATUS_OUT;
			USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
		}
		break;

	case STATUS_OUT:

		if( (ch_ep & USB_EP_SETUP) != 0 || (istr & USB_ISTR_DIR) == 0 )
			error(__FUNCTION__,-6);
		USB_DRD_CLEAR_RX_CHEP_CTR(USB_DRD_FS, 0);

		STRPRINT(" STATUS OUT, count_rx=%d, dev_addr=0x%02X\n",USB_DRD_GET_CHEP_RX_CNT(USB_DRD_FS,0), USB_DRD_FS->DADDR & 0x7F);

		/* When the out status transaction is serviced, the application clears the STATUS_OUT bit
		 * and sets STATRX to VALID (to accept a new command) and STATTX to NAK (to delay a
		 * possible status stage immediately following the next setup).
		 */
		USB_DRD_CLEAR_CHEP_KIND(USB_DRD_FS,0);
		ep_state[0] = SETUP;
		USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,0,USB_EP_TX_NAK);
		USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
		break;

	case DATA_OUT:
		if( (ch_ep & USB_EP_SETUP) != 0 || (istr & USB_ISTR_DIR) == 0 )
			error(__FUNCTION__,-7);

		USB_DRD_CLEAR_RX_CHEP_CTR(USB_DRD_FS, 0);

		STRPRINT(" DATA OUT, count_rx=%d, dev_addr=0x%02X\n",USB_DRD_GET_CHEP_RX_CNT(USB_DRD_FS,0), USB_DRD_FS->DADDR & 0x7F);
		ep_state[0] = SETUP;
		USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
		break;

	case STATUS_IN:
		if( (ch_ep & USB_EP_SETUP) != 0 || (istr & USB_ISTR_DIR) != 0 )
			error(__FUNCTION__,-8);

		USB_DRD_CLEAR_TX_CHEP_CTR(USB_DRD_FS, 0);

		STRPRINT(" STATUS IN, count_tx=%d, dev_addr=0x%02X\n",USB_DRD_GET_CHEP_TX_CNT(USB_DRD_FS,0), USB_DRD_FS->DADDR & 0x7F);

		/*
		 * Once the SET_ADDRESS request has completed successfully, change the device address
		 */
		if(change_address != 0) {
			if(received_dev_address == 0) {
				dev_state = USB_DEFAULT;
			}
			else {
				dev_state = USB_ADDRESS;
			}
			USB_DRD_FS->DADDR = 0x80 | received_dev_address;
			change_address = 0;
		}
		ep_state[0] = SETUP;
		USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,0,USB_EP_RX_VALID);
		break;

	default:
		error(__FUNCTION__,-9);
	}

	return 0;
}

/*
 * EP1 is dedicated to GPIOs
 */
int ep1_sm(uint32_t istr)
{
	uint32_t xfer_count;
	int ep_num=1;

	switch(ep_state[ep_num]) {

	case EP_REQ:
		USB_DRD_CLEAR_RX_CHEP_CTR(USB_DRD_FS, ep_num);
		if((istr & USB_ISTR_DIR) == 0) {
			error(__FUNCTION__,-1);
			stall_ep(ep_num);
			return -1;
		}
		xfer_count = (uint16_t)USB_DRD_GET_CHEP_RX_CNT(USB_DRD_FS, ep_num);

		STRPRINT("Received %d bytes. Operation: %d, pin %c%d\n",xfer_count,gpio_request.operation,gpio_request.port,gpio_request.pin);

		USB_ReadPMA(USB_DRD_FS, (uint8_t*)&gpio_request, ch_ep_out[ep_num].pmaadress, (uint16_t)xfer_count);
		switch(gpio_request.operation) {
		case GPIO_CLEAR:
			gpio_clear(gpio_request.port,gpio_request.pin);
			USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,ep_num,USB_EP_RX_VALID);
			break;
		case GPIO_SET:
			gpio_set(gpio_request.port,gpio_request.pin);
			USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,ep_num,USB_EP_RX_VALID);
			break;
		case GPIO_GET:
			int v = gpio_get(gpio_request.port,gpio_request.pin);
			ep_remaining_bytes[ep_num] = sizeof(v);
			ep_data_p[ep_num] = (uint8_t*)&v;
			ep_state[ep_num] = EP_IN;
			xfer_count = min(ep_remaining_bytes[ep_num],EP_MAX_PACKET_SIZE);
			USB_WritePMA(USB_DRD_FS,ep_data_p[ep_num], ch_ep_in[ep_num].pmaadress, xfer_count);
			USB_DRD_SET_CHEP_TX_CNT(USB_DRD_FS,ep_num,xfer_count);
			USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,ep_num,USB_EP_TX_VALID);
			break;
		case GPIO_CONFIG:
			gpio_config(gpio_request.port, gpio_request.pin, gpio_request.direction, gpio_request.type,gpio_request.pull);
			USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,ep_num,USB_EP_RX_VALID);
			break;
		default:
			/* call a gpio function with intentionally wrong parameter to set the gpio errno = GPIO_ERR_PARAMETER */
			gpio_get(0,0);
			USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,ep_num,USB_EP_RX_VALID);
			break;
		}
		break;
	case EP_IN:
		USB_DRD_CLEAR_TX_CHEP_CTR(USB_DRD_FS, ep_num);
		if((istr & USB_ISTR_DIR) != 0) {
			error(__FUNCTION__,-2);
			stall_ep(ep_num);
		}
		xfer_count = (uint16_t)USB_DRD_GET_CHEP_TX_CNT(USB_DRD_FS, ep_num);
		STRPRINT("Transmitted %d bytes\n",xfer_count);
		ep_state[ep_num] = EP_REQ;
		USB_DRD_SET_CHEP_TX_STATUS(USB_DRD_FS,ep_num,USB_EP_TX_NAK);
		USB_DRD_SET_CHEP_RX_STATUS(USB_DRD_FS,ep_num,USB_EP_RX_VALID);
		break;
	default:
		break;
	}

	return 0;
}

int ctr_isr()
{
	uint16_t istr;
	uint8_t idn;
	uint32_t res=0;
	int loop=0;

	while( (USB_DRD_FS->ISTR & USB_ISTR_CTR) != 0) {
		istr = (uint16_t)(USB_DRD_FS->ISTR);
		// Extract endpoint
		idn = (uint8_t)(istr & USB_ISTR_IDN);

		uint16_t ch_ep = (uint16_t)USB_DRD_GET_CHEP(USB_DRD_FS,idn);
		STRPRINT("TS: %d: CH_EP%d=0x%4X ",(TIM5->CNT)>>2, idn, ch_ep);

		switch(idn) {
		case 0:
			res = ep0_sm(istr);
			if(res == -1)
				__NOP();
			break;
		case 1:
			res = ep1_sm(istr);
			if(res == -1)
				__NOP();
			break;
		default:
			break;
		}
		if(++loop > 10)
			error(__FUNCTION__,-20);
	}
	return 0;
}

void USB_Init()
{
	USB_ll_init();
	set_serial_number();

	dev_state = USB_POWERED;
}
