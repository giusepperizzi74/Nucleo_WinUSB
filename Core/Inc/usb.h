/*
 * usb.h
 *
 *  Created on: Feb 16, 2025
 *      Author: girizzi
 */

#ifndef INC_USB_H_
#define INC_USB_H_

#include "usb_ll.h"

struct __attribute__((packed)) usb_request {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
};


/* Descriptor types USB Spec 2.0*/
#define DESCR_DEVICE							1
#define DESCR_CONFIGURATION						2
#define DESCR_STRING							3
#define DESCR_INTERFACE							4
#define DESCR_ENDPOINT							5
#define DESCR_DEVICE_QUALIFER					6
#define DESCR_OTHER_SPEED_CONFIGURATION			7
#define DESCR_INTERFACE_POWER					8
/* Additional Descriptor Types in USB Spec 3.0 */
#define DESCR_OTG								9
#define DESCR_DEBUG								10
#define DESCR_INTERFACE_ASSOCIATION				11
#define DESCR_BOS								15
#define DESCR_DEVICE_CAPABILITY					16
#define DESCR_SUPERSPEED_USB_ENDPOINT_COMPANION	48

void usb_reset_isr();
int usb_ctr_isr();
int ctr_isr();
void USB_Init();

#endif /* INC_USB_H_ */
