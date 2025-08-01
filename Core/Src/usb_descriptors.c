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

#include "usb_descriptors.h"
#include "usb.h"

const struct usb_device_descriptor usb_device_desc = {
	.bLength=18,
	.bDescriptorType = 1,
	.bcdUSB = 0x0210,
	.bDeviceClass = 0x00,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0x00,
	.bMaxPacketSize0 = EP_MAX_PACKET_SIZE,
	.idVendor = 0x0483,
	.idProduct = 0x0001,
	.bcdDevice = 0x0001,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1
};

struct usb_framework_descriptor usb_framework_desc = {
	.configuration.bLength = 9,
	.configuration.bDescriptorType = DESCR_CONFIGURATION,
	.configuration.wTotalLength = 18+7*(BULK_ENDPOINT_COUNT),	// assuming only 1 interface descriptor after the configuration descriptor plus 6 endpoint descriptors
	.configuration.bNumInterfaces = 1,
	.configuration.bConfigurationValue = 1,
	.configuration.iConfiguration = 4,
	.configuration.bmAttributes = 0xC0,
	.configuration.bMaxPower = 0,

	.interface.bLength = 9,
	.interface.bDescriptorType = DESCR_INTERFACE,
	.interface.bInterfaceNumber = 0,
	.interface.bAlternateSetting = 0,
	.interface.bNumEndpoints = BULK_ENDPOINT_COUNT,	// .bNumEndpoint reports the number of endpoints except the default control endpoints
	.interface.bInterfaceClass = 0xFF,
	.interface.bInterfaceSubClass = 0xFF,
	.interface.bInterfaceProtocol = 0xFF,
	.interface.iInterface = 0,

	.endpoints[0].bLength = 7,
	.endpoints[0].bDescriptorType = DESCR_ENDPOINT,
	.endpoints[0].bEndpointAddress = 0x01,	// OUT Endpoint
	.endpoints[0].bmAttributes = 0x02, 		// Bulk Transfer
	.endpoints[0].wMaxPacketSize = EP_MAX_PACKET_SIZE,
	.endpoints[0].bInterval = 1,

	.endpoints[1].bLength = 7,
	.endpoints[1].bDescriptorType = DESCR_ENDPOINT,
	.endpoints[1].bEndpointAddress = 0x81,	// IN Endpoint
	.endpoints[1].bmAttributes = 0x02, 		// Bulk Transfer
	.endpoints[1].wMaxPacketSize = EP_MAX_PACKET_SIZE,
	.endpoints[1].bInterval = 1,
};

// USB strings must be UTF-16
const uint8_t language_string_descriptor[] = {
	4,
	DESCR_STRING,
	0x09,
	0x04
};
const uint8_t manufacturer_string_descriptor[] = {
	38,
	DESCR_STRING,
	'S',0x0,
	'T',0x0,
	'M',0x0,
	'i',0x0,
	'c',0x0,
	'r',0x0,
	'o',0x0,
	'e',0x0,
	'l',0x0,
	'e',0x0,
	'c',0x0,
	't',0x0,
	'r',0x0,
	'o',0x0,
	'n',0x0,
	'i',0x0,
	'c',0x0,
	's',0x0};

const uint8_t product_string_descriptor[] = {
	28,				// total length
	DESCR_STRING,	// descriptor type
	'N',0x0,
	'u',0x0,
	'c',0x0,
	'l',0x0,
	'e',0x0,
	'o',0x0,
	'-',0x0,
	'W',0x0,
	'i',0x0,
	'n',0x0,
	'U',0x0,
	'S',0x0,
	'B',0x0};

uint8_t serial_string_descriptor[] = {
	50,
	DESCR_STRING,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00,
	'0',0x00};

const uint8_t configuration_string_descriptor[] = {
	32,
	DESCR_STRING,
	'C',0x00,
	'o',0x00,
	'n',0x00,
	'f',0x00,
	'i',0x00,
	'g',0x00,
	'u',0x00,
	'r',0x00,
	'a',0x00,
	't',0x00,
	'i',0x00,
	'o',0x00,
	'n',0x00,
	'0',0x00,
	'1',0x00};


const struct usb_OS_string_descriptor usb_OS_string_desc = {
	.bLength = 18,
	.bDescriptorType = 0x03,
	.qwSignature = {0x4D, 0x00, 0x53, 0x00, 0x46, 0x00, 0x54, 0x00, 0x31, 0x00, 0x30, 0x00, 0x30, 0x00 },
	.bMS_VendorCode = REQUEST_GET_MS_DESCRIPTOR,
	.bPad = 0x00
};

const struct usb_ms_compatible_ID_feature_descriptor usb_ms_compatible_ID_feature_desc = {
	.dwLength = 40,
	.bcdVersion = 0x0100,
	.wIndex = 0x0004,
	.bCount = 1,
	.reserved1 = {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	.bFirstInterfaceNumber = 0,
	.reserved2 = 0x01,
	.compatibleID = {'W','I','N','U','S','B',0x00,0x00},
	.subCompatibleID = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	.reserved3 = {0x00,0x00,0x00,0x00,0x00,0x00}
};


uint8_t usb_extended_property_feature_desc[EXTENDED_PROPERTY_FEATURE_DESCRIPTOR_LENGTH] = {
/* dwLength : Descriptor length */
EXTENDED_PROPERTY_FEATURE_DESCRIPTOR_LENGTH,0x00,0x00,0x00,
/* bcdVersion : Version (1.0) */
0x00,0x01,
/* wIndex : Extended Property Descriptor index (5) */
0x05,0x00,
/* wCount : Number of sections (1) */
0x01,0x00,
/* dwSize : Size of the property section (132 bytes) */
132,0x00,0x00,0x00,
/* dwPropertyDataType : Property data type (1 = Unicode REG_SZ) */
0x01,0x00,0x00,0x00,
/* wPropertyNameLength : Property name length (40 bytes) */
40,0x00,
/* bPropertyName : Property name "DeviceInterfaceGUID" */
'D',0x00,
'e',0x00,
'v',0x00,
'i',0x00,
'c',0x00,
'e',0x00,
'I',0x00,
'n',0x00,
't',0x00,
'e',0x00,
'r',0x00,
'f',0x00,
'a',0x00,
'c',0x00,
'e',0x00,
'G',0x00,
'U',0x00,
'I',0x00,
'D',0x00,
0x00,0x00,
/* dwPropertyDataLength : Property data length (78 bytes) */
78,0x00,0x00,0x00,
/* bPropertyData : Property value */
'{',0x00,
'e',0x00,
'c',0x00,
'c',0x00,
'e',0x00,
'f',0x00,
'f',0x00,
'3',0x00,
'5',0x00,
'-',0x00,
'1',0x00,
'4',0x00,
'6',0x00,
'c',0x00,
'-',0x00,
'4',0x00,
'f',0x00,
'f',0x00,
'3',0x00,
'-',0x00,
'a',0x00,
'c',0x00,
'd',0x00,
'9',0x00,
'-',0x00,
'8',0x00,
'f',0x00,
'9',0x00,
'9',0x00,
'2',0x00,
'd',0x00,
'0',0x00,
'9',0x00,
'a',0x00,
'c',0x00,
'd',0x00,
'd',0x00,
'}',0x00,
0x00,0x00
};



// Micrsoft OS 2.0 Descriptor Set for a non-composite device.
uint8_t msOs20DescriptorSet[MS_OS_20_LENGTH] =
{
    // Microsoft OS 2.0 Descriptor Set header (Table 10)
    10, 0x00,  							// wLength : The length in bytes, of this hader. Shall be set to 10.
    MS_OS_20_SET_HEADER_DESCRIPTOR, 0x00,	// wDescriptorType (=0)
    0x00, 0x00, 0x03, 0x06,  				// dwWindowsVersion: Windows 8.1 (NTDDI_WINBLUE)
    MS_OS_20_LENGTH, 0x00,  				// wTotalLength : The size of entire MS OS 2.0 descriptor set. The value shall match the value in the descriptor set information structure

    // Microsoft OS 2.0 compatible ID descriptor (Table 13)
    20, 0x00,                                      // wLength : The length, in bytes, of the compatible ID descriptor including value descriptor. Shall be set to 20.
    MS_OS_20_FEATURE_COMPATIBLE_ID, 0x00,            // wDescriptorType (=3)
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,        // CompatibleID String
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // SubCompatibleID

    // Microsoft OS 2.0 registry property descriptor (Table 14)
    132, 0x00,   									// wLength : The length, in bytes, of this descriptor
    MS_OS_20_FEATURE_REG_PROPERTY, 0x00,			// wDescriptorType (=4)
	7, 0x00,										// wPropertyDataType:  REG_MULTI_SZ (See table 15)
    42, 0x00,   									// wPropertyNameLength : The length of the property name
    'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,'I',0,'n',	// PropertName : The Name of the registry property
	0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,'G',0,'U',0,'I',0,'D',0,'s',0,0,0,
    80, 0x00,   									// wPropertyDataLength : The length of the property data

	'{',0,'7',0,'f',0,'c',0,'2',0,'4',0,'e',0,'e',0,'3',0,'-',0,	//{7fc24ee3-4466-430b-af12-08c596810b29}
	'4',0,'4',0,'6',0,'6',0,'-',0,'4',0,'3',0,'0',0,'b',0,'-',0,
	'a',0,'f',0,'1',0,'2',0,'-',0,'0',0,'8',0,'c',0,'5',0,'9',0,
	'6',0,'8',0,'1',0,'0',0,'b',0,'2',0,'9',0,'}',0,0x00,0x00,0x00,0x00,
};


uint8_t bosDescriptor[33] =
{
    0x05,       							// bLength : Size of descriptor
    USB_DESCRIPTOR_TYPE_BOS,				// bDescriptorType : BOS Descriptor type (=15)
    0x21, 0x00, 							// wTotalLength : Length of this descriptor and all of its descriptors
    0x01,       							// bNumDeviceCaps : The number of separate device capability descriptors in the BOS

	/* Device Capability Descriptor Header */
    0x1C,       							// bLength : Size of this descriptor
    USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY,	// bDescriptorType : Descriptor type : DEVICE CAPABILITY (=16)
    USB_DEVICE_CAPABILITY_TYPE_PLATFORM,	// bDevCapabilityType : Valid values are listed in Table 9-11 of USB Spec3.0 (=5)

	/* Device Capability Descriptor */
    0x00,       							// bReserved

											// PlatformCapabilityUUID
    										// Microsoft OS 2.0 descriptor platform capability UUID
    										// from Microsoft OS 2.0 Descriptors,  Table 3.
    0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C,
    0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F,

	/* Capability Data */
    0x00, 0x00, 0x03, 0x06,  				// dwWindowsVersion: Windows 8.1 (NTDDI_WINBLUE)
    MS_OS_20_LENGTH, 0x00,              	// wMSOSDescriptorSetTotalLength
	REQUEST_GET_MS_DESCRIPTOR,				// bMS_VendorCode : Vendor-defined code to use to retrieve this version of the MS OS 2.0 descriptor
    0,                       				// bAltEnumCode : A non-zero value to send to the device to indicate that the device may return non-default USB descriptor for enumeration
};


usb_indexed_string_t usb_strings[] = {
	{0,language_string_descriptor},
	{1,manufacturer_string_descriptor},
	{2,product_string_descriptor},
	{3,serial_string_descriptor},
	{4,configuration_string_descriptor},
	{0xEE,(uint8_t*)&usb_OS_string_desc},
	{0xFF,0}
};

static char dec2hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void set_serial_number()
{
	// set the USB serial number from the MCU serial number
	uint32_t tmp = *((uint32_t*)0x08FFF800);
	for(int i=0;i<8;i++)
		serial_string_descriptor[2+2*i] = dec2hex[tmp>>(4*i) & 0xF];
	tmp = *((uint32_t*)0x08FFF804);
	for(int i=0;i<8;i++)
		serial_string_descriptor[18+2*i] = dec2hex[tmp>>(4*i) & 0xF];
	tmp = *((uint32_t*)0x08FFF808);
	for(int i=0;i<8;i++)
		serial_string_descriptor[34+2*i] = dec2hex[tmp>>(4*i) & 0xF];
}
