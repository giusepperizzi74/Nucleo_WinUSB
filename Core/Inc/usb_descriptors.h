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

#ifndef INC_USB_DESCRIPTORS_H_
#define INC_USB_DESCRIPTORS_H_

#include <stdint.h>

#define EP_MAX_PACKET_SIZE		64
#define CONTROL_ENDPOINT_COUNT	2
#define BULK_ENDPOINT_COUNT		2
#define TOT_ENDPOINT_COUNT		(CONTROL_ENDPOINT_COUNT + BULK_ENDPOINT_COUNT)

struct __attribute__((packed)) usb_device_descriptor {

	/* Size of this descriptor in bytes */
	uint8_t bLength;

	/* DEVICE descriptor type */
	uint8_t bDescriptorType;

	/* 	USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H).
		This field identifies the release of the USB Specification with which the device and its
		descriptors are compliant */
	uint16_t bcdUSB;

	/* Class code (assigned by the USB-IF). If this field is reset to zero, each interface
	within a configuration specifies its own class information and the various
	interfaces operate independently. If this field is set to a value between 1 and
	FEH, the device supports different class specifications on different interfaces and
	the interfaces may not operate independently. This value identifies the
	class definition used for the aggregate interfaces.
	If this field is set to FFH, the device class is vendor-specific. */
	uint8_t bDeviceClass;

	/* Subclass code (assigned by the USB-IF). These codes are qualified by the value of
	the bDeviceClass field. If the bDeviceClass field is reset to zero,
	this field must also be reset to zero. If the bDeviceClass field is not set to FFH,
	all values are reserved for assignment by the USB-IF. */
	uint8_t bDeviceSubClass;

	/* Protocol code (assigned by the USB-IF). These codes are qualified by the value of
	the bDeviceClass and the bDeviceSubClass fields. If a device
	supports class-specific protocols on a device basis as opposed to an interface
	basis, this code identifies the protocols that the device uses as defined by the
	specification of the device class. If this field is reset to zero, the device
	does not use class-specific protocols on a device basis. However, it may use classspecific
	protocols on an interface basis. If this field is set to FFH, the device uses a
	vendor-specific protocol on a device basis. */
	uint8_t bDeviceProtocol;

	/* Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid) */
	uint8_t bMaxPacketSize0;

	/* Vendor ID (assigned by the USB-IF) */
	uint16_t idVendor;

	/* Product ID (assigned by the manufacturer) */
	uint16_t idProduct;

	/* Device release number in binary-coded decimal */
	uint16_t bcdDevice;

	/* Index of string descriptor describing manufacturer */
	uint8_t iManufacturer;

	/* Index of string descriptor describing product */
	uint8_t iProduct;

	/* Index of string descriptor describing the device’s serial number */
	uint8_t iSerialNumber;

	/* Number of possible configurations */
	uint8_t bNumConfigurations;
};

struct __attribute__((packed)) usb_device_qualifier_descriptor {

	/* Size of this descriptor in bytes */
	uint8_t bLength;

	/* DEVICE descriptor type */
	uint8_t bDescriptorType;

	/* USB specification version number (e.g., 0200H for V2.00 ) */
	uint16_t bcdUSB;

	/* Class Code */
	uint8_t bDeviceClass;

	/* SubClass Code */
	uint8_t bDeviceSubClass;

	/* Protocol Code */
	uint8_t bDeviceProtocol;

	/* Maximum packet size for other speed */
	uint8_t bMaxPacketSize0;

	/* Number of Other-speed Configurations */
	uint8_t bNumConfigurations;

	/* Reserved for future use, must be zero */
	uint8_t bReserved;
};

struct __attribute__((packed)) usb_configuration_descriptor {

	/* Size of this descriptor in bytes */
	uint8_t bLength;

	/* CONFIGURATION descriptor type */
	uint8_t bDescriptorType;

	/* Total length of data returned for this configuration. Includes the combined length
	of all descriptors (configuration, interface, endpoint, and class- or vendor-specific)
	returned for this configuration. */
	uint16_t wTotalLength;

	/* Number of interfaces supported by this configuration */
	uint8_t bNumInterfaces;

	/* Value to use as an argument to the SetConfiguration() request to select this
	configuration */
	uint8_t bConfigurationValue;

	/* Index of string descriptor describing this configuration */
	uint8_t iConfiguration;

	/* Configuration characteristics
	D7: Reserved (set to one)
	D6: Self-powered
	D5: Remote Wakeup
	D4...0: Reserved (reset to zero)
	D7 is reserved and must be set to one for historical reasons.
	A device configuration that uses power from the bus and a local source reports a non-zero
	value in bMaxPower to indicate the amount of bus power required and sets D6. The actual
	power source at runtime may be determined using the GetStatus(DEVICE) request (see
	Section 9.4.5). If a device configuration supports remote wakeup, D5 is set to one. */
	uint8_t bmAttributes;

	/* Maximum power consumption of the USB device from the bus in this specific
	configuration when the device is fully operational. Expressed in 2 mA units
	(i.e., 50 = 100 mA).
	Note: A device configuration reports whether the configuration is bus-powered or selfpowered.
	Device status reports whether the device is currently self-powered. If a device is
	disconnected from its external power source, it updates device status to indicate that it is no
	longer self-powered. A device may not increase its power draw
	from the bus, when it loses its external power source, beyond the amount reported by its
	configuration. If a device can continue to operate when disconnected from its external power source, it
	continues to do so. If the device cannot continue to operate, it fails operations it can
	no longer support. The USB System Software may determine the cause of the failure by
	checking the status and noting the loss of the device’s power source. */
	uint8_t bMaxPower;
};

struct __attribute__((packed)) usb_interface_descriptor {

	/* Size of this descriptor in bytes */
	uint8_t bLength;

	/* INTERFACE descriptor type */
	uint8_t bDescriptorType;

	/* Number of this interface. Zero-based value identifying the index in the array of
	concurrent interfaces supported by this configuration. */
	uint8_t bInterfaceNumber;

	/* Value used to select this alternate setting for the interface identified in the prior field */
	uint8_t bAlternateSetting;

	/* Number of endpoints used by this interface (excluding endpoint zero). If this
	value is zero, this interface only uses the Default Control Pipe. */
	uint8_t bNumEndpoints;

	/* Class code (assigned by the USB-IF). A value of zero is reserved for future
	standardization. If this field is set to FFH, the interface class is vendor-specific.
	All other values are reserved for assignment by the USB-IF. */
	uint8_t bInterfaceClass;

	/* Subclass code (assigned by the USB-IF). These codes are qualified by the value of
	the bInterfaceClass field. If the bInterfaceClass field is reset to zero,
	this field must also be reset to zero. If the bInterfaceClass field is not set to
	FFH, all values are reserved for assignment by the USB-IF. */
	uint8_t bInterfaceSubClass;

	/* Protocol code (assigned by the USB). These codes are qualified by the value of
	the bInterfaceClass and the bInterfaceSubClass fields. If an interface
	supports class-specific requests, this code identifies the protocols that the device
	uses as defined by the specification of the device class.
	If this field is reset to zero, the device does not use a class-specific protocol on
	this interface. If this field is set to FFH, the device uses
	a vendor-specific protocol for this interface. */
	uint8_t bInterfaceProtocol;

	/* Index of string descriptor describing this interface */
	uint8_t iInterface;
};

struct __attribute__((packed)) usb_endpoint_descriptor {

	/* Size of this descriptor in bytes */
	uint8_t bLength;

	/* ENDPOINT descriptor type */
	uint8_t bDescriptorType;

	/* The address of the endpoint on the USB device described by this descriptor. The address is
	encoded as follows:
	Bit 3...0: The endpoint number
	Bit 6...4: Reserved, reset to zero
	Bit 7: Direction, ignored for 	control endpoints
		0 = OUT endpoint
		1 = IN endpoint
	*/
	uint8_t bEndpointAddress;

	/* This field describes the endpoint’s attributes when it is configured using the bConfigurationValue.
	Bits 1..0: Transfer Type
		00 = Control
		01 = Isochronous
		10 = Bulk
		11 = Interrupt
	If not an isochronous endpoint, bits 5..2 are reserved and must be set to zero. If isochronous, they are
	defined as follows:
	Bits 3..2: Synchronization Type
		00 = No Synchronization
		01 = Asynchronous
		10 = Adaptive
		11 = Synchronous
	Bits 5..4: Usage Type
		00 = Data endpoint
		01 = Feedback endpoint
		10 = Implicit feedback Data endpoint
		11 = Reserved
	Refer to Chapter 5 for more information.
	All other bits are reserved and must be reset to zero.
	Reserved bits must be ignored by the host.
	*/
	uint8_t bmAttributes;

	/* Maximum packet size this endpoint is capable of 	sending or receiving when this configuration is selected.
	For isochronous endpoints, this value is used to reserve the bus time in the schedule, required for the
	per-(micro)frame data payloads. The pipe may, on an ongoing basis, actually use less bandwidth than that
	reserved. The device reports, if necessary, the actual bandwidth used via its normal, non-USB defined
	mechanisms.
	For all endpoints, bits 10..0 specify the maximum packet size (in bytes).
	For high-speed isochronous and interrupt endpoints:
	Bits 12..11 specify the number of additional transaction opportunities per microframe:
		00 = None (1 transaction per microframe)
		01 = 1 additional (2 per microframe)
		10 = 2 additional (3 per microframe)
		11 = Reserved
	Bits 15..13 are reserved and must be set to zero.
	Refer to Chapter 5 for more information.
	*/
	uint16_t wMaxPacketSize;

	/* Interval for polling endpoint for data transfers. 	Expressed in frames or microframes depending on the
	device operating speed (i.e., either 1 millisecond or 125 μs units).
	For full-/high-speed isochronous endpoints, this value 	must be in the range from 1 to 16. The bInterval value
	is used as the exponent for a 2bInterval-1 value; e.g., a 	bInterval of 4 means a period of 8 (24-1).
	For full-/low-speed interrupt endpoints, the value of this field may be from 1 to 255.
	For high-speed interrupt endpoints, the bInterval value is used as the exponent for a 2bInterval-1 value; e.g., a
	bInterval of 4 means a period of 8 (24-1). This value must be from 1 to 16.
	For high-speed bulk/control OUT endpoints, the bInterval must specify the maximum NAK rate of the
	endpoint. A value of 0 indicates the endpoint never NAKs. Other values indicate at most 1 NAK each
	bInterval number of microframes. This value must be in the range from 0 to 255.
	See Chapter 5 description of periods for more detail.
	*/
	uint8_t bInterval;
};

struct __attribute__((packed)) usb_framework_descriptor {
	struct usb_configuration_descriptor configuration;
	struct usb_interface_descriptor interface;
	struct usb_endpoint_descriptor endpoints[BULK_ENDPOINT_COUNT];
};

struct __attribute__((packed)) usb_OS_string_descriptor {

	/* Size of this descriptor in bytes */
	uint8_t bLength;

	/* ENDPOINT descriptor type */
	uint8_t bDescriptorType;

	/* contains a Unicode character array that identifies the descriptor as an OS string descriptor
	 * and includes the version number. For version 1.00, this array must be set to ‘MSFT100’
	 * (0x4D00 0x5300 0x4600 0x5400 0x3100 0x3000 0x3000). */
	uint8_t qwSignature[14];

	/* this value is used to retrieve the associated feature descriptors: the bRequest field in the
	 * OS Feature Description request must be set to this value */
	uint8_t bMS_VendorCode;

	uint8_t bPad;
};

struct __attribute__((packed)) usb_ms_compatible_ID_feature_descriptor {

	/*** Header ***/

	/* Size of this descriptor in bytes */
	uint32_t dwLength;

	/* OS Description version */
	uint16_t bcdVersion;

	/* Extended compact ID descriptor */
	uint16_t wIndex;

	/* Number of function sections */
	uint8_t bCount;

	uint8_t reserved1[7];

	/*** Function ***/

	/* bFirstInterfaceNumber*/
	uint8_t bFirstInterfaceNumber;

	/* Must be = 0x01 */
	uint8_t reserved2;

	/* strings must be UTF-16 */
	uint8_t compatibleID[8];

	uint8_t subCompatibleID[8];

	uint8_t reserved3[6];
};

struct __attribute__((packed)) usb_extended_property_os_feature_descriptor {

	/*** Header ***/

	/* Size of this descriptor in bytes */
	uint32_t dwLength;

	/* OS Description version */
	uint16_t bcdVersion;

	/* Extended compact ID descriptor */
	uint16_t wIndex;

	/* Number of custom property sections that follow the header section */
	uint16_t wCount;

	/*** Custom properties ***/

	/* Size of this custom property section */
	uint32_t dwSize;

	/* Property data format */
	uint32_t dwPropertyDataType;

	/* Property name length */
	uint16_t wPropertyNameLength;

	/* Property name */
	uint8_t* bPropertyName;

	/* Length of the buffer holding the property data */
	uint32_t dwPropertyDataLength;

	/* Property data */
	uint8_t* bPropertyData;
};

typedef struct {
	int index;
	const uint8_t* ptr;
}usb_indexed_string_t;

extern usb_indexed_string_t usb_strings[];

extern const struct usb_device_descriptor usb_device_desc;
extern struct usb_device_qualifier_descriptor usb_device_qual_desc;

extern struct usb_framework_descriptor usb_framework_desc;
extern const struct usb_OS_string_descriptor usb_OS_string_desc;
extern const struct usb_ms_compatible_ID_feature_descriptor usb_ms_compatible_ID_feature_desc;
#define EXTENDED_PROPERTY_FEATURE_DESCRIPTOR_LENGTH 142
extern uint8_t usb_extended_property_feature_desc[EXTENDED_PROPERTY_FEATURE_DESCRIPTOR_LENGTH];


#define REQUEST_GET_MS_DESCRIPTOR    0x01

// Wireless USB Specification 1.1, Table 7-1
#define USB_DESCRIPTOR_TYPE_SECURITY 12
#define USB_DESCRIPTOR_TYPE_KEY 13
#define USB_DESCRIPTOR_TYPE_ENCRYPTION_TYPE 14
#define USB_DESCRIPTOR_TYPE_BOS 15
#define USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY 16
#define USB_DESCRIPTOR_TYPE_WIRELESS_ENDPOINT_COMPANION 17

// Microsoft OS 2.0 Descriptors, Table 1
#define USB_DEVICE_CAPABILITY_TYPE_PLATFORM 5

// Microsoft OS 2.0 Descriptors, Table 8
#define MS_OS_20_DESCRIPTOR_INDEX 7
#define MS_OS_20_SET_ALT_ENUMERATION 8

// Microsoft OS 2.0 Descriptors, Table 9
#define MS_OS_20_SET_HEADER_DESCRIPTOR 0x00
#define MS_OS_20_SUBSET_HEADER_CONFIGURATION 0x01
#define MS_OS_20_SUBSET_HEADER_FUNCTION 0x02
#define MS_OS_20_FEATURE_COMPATIBLE_ID 0x03
#define MS_OS_20_FEATURE_REG_PROPERTY 0x04
#define MS_OS_20_FEATURE_MIN_RESUME_TIME 0x05
#define MS_OS_20_FEATURE_MODEL_ID 0x06
#define MS_OS_20_FEATURE_CCGP_DEVICE 0x07

#define MS_OS_20_LENGTH 162

extern uint8_t msOs20DescriptorSet[MS_OS_20_LENGTH];
extern uint8_t bosDescriptor[0x21];
extern void set_serial_number();
//extern const uint8_t framework[34];
#endif /* INC_USB_DESCRIPTORS_H_ */
