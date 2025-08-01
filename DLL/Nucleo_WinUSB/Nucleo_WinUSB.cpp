#include <windows.h>
#include <cfgmgr32.h>
#include <winusb.h>
#include <stdint.h>
#include "..\Nucleo_WinUSB.h"

#define SUCCESS								0

#pragma pack(push,1) // Set packing alignemnt to 1 byte
enum operation_type {

	/* gpio */
	GPIO_SET = 0x0200,
	GPIO_CLEAR,
	GPIO_GET,
	GPIO_CONFIG,

	NO_OP = 0xFFFF
};

struct gpio_request_t {
	uint32_t operation;
	uint8_t port;
	uint8_t pin;
	uint8_t direction;
	uint8_t type;
	uint8_t pull;
};

constexpr int max_num_of_interfaces{ 1 };

struct Device {
	char name[256];
	HANDLE file_handle;
	WINUSB_INTERFACE_HANDLE interface_handles[max_num_of_interfaces];
	size_t setup_pckt_size;

	Device();
	//Device(char* descr);
	~Device();

	void* open(char* descr);
};
#pragma pack(pop)	// restore

Device::Device()
{
	name[0] = '\0';
	file_handle = NULL;
	for (int i = 0; i < max_num_of_interfaces; i++)
		interface_handles[i] = NULL;
	setup_pckt_size = 64;
}

void* Device::open(char* descr)
{
	if (descr == NULL || strlen(descr) > sizeof(Device::name))
		return NULL;

	HANDLE fh = CreateFileA(descr, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (INVALID_HANDLE_VALUE == fh)
		return NULL;

	WINUSB_INTERFACE_HANDLE ih;
	if (WinUsb_Initialize(fh, &ih) != TRUE) {
		CloseHandle(fh);
		return NULL;
	}

	name[0] = '\0';
	strncpy_s(name, descr, sizeof(Device::name));
	file_handle = fh;
	interface_handles[0] = ih;
	setup_pckt_size = 64;

	return this;
}

Device::~Device()
{
	for (int i = 1; i < max_num_of_interfaces; i++) {
		if (interface_handles[i] != NULL)
			WinUsb_Free(interface_handles[i]);
	}

	if (interface_handles[0] != NULL)
		WinUsb_Free(interface_handles[0]);

	interface_handles[0] = NULL;
	if (file_handle != NULL)
		CloseHandle(file_handle);
	file_handle = NULL;
	name[0] = '\0';
}

constexpr char gpio_pipe_id = 0x01;
char device_list[1024];

Device device;

char* get_next(char* desc)
{
	CONFIGRET cr;
	GUID guidDeviceInterface = { 0x7fc24ee3, 0x4466, 0x430b,{ 0xaf,0x12,0x08,0xc5,0x96,0x81,0x0b,0x29} };

	if (desc == NULL) {
		cr = CM_Get_Device_Interface_ListA(&guidDeviceInterface, NULL, device_list, sizeof(device_list), CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
		if (cr != CR_SUCCESS) {
			return NULL;
		}
		desc = device_list;
	}
	else {
		desc = desc + strlen(desc) + 1;
	}
	if (*desc == 0)
		return 0;
	return desc;
}

/*
* Interface 0 must be the first to be opened.
* Other interfaces of the same devices can be opened after opening interface 0
*/
void* open(char* desc)
{
	if (device.file_handle == NULL)
		return device.open(desc);

	return NULL;
}

int open_interface(void* handle, int if_num)
{
	Device* h = (Device*)handle;
	if (h == NULL || h->interface_handles[0] == NULL)
		return -1;
	if (if_num <= 0 || if_num >= max_num_of_interfaces || h->interface_handles[if_num] != NULL)
		return -2;

	WINUSB_INTERFACE_HANDLE ih;
	if (WinUsb_GetAssociatedInterface(h->interface_handles[0], if_num, &ih) == TRUE) {
		h->interface_handles[if_num] = ih;
		return 0;
	}
	return -3;
}

/*
* All interfaces numbers > 0 must be closed with close_interface() before calling close()
*/
int close(void* handle)
{
	Device* h = (Device*)handle;

	if (h == NULL)
		return SUCCESS;

	// check that all other interfaces are closed
	int i;
	for (i = 1; i < max_num_of_interfaces; i++) {
		if (h->interface_handles[i] != NULL)
			return -1;
	}

	WinUsb_Free(h->interface_handles[0]);
	h->interface_handles[0] = NULL;
	CloseHandle(h->file_handle);
	h->file_handle = NULL;

	return SUCCESS;
}

int close_interface(void* handle, int if_num)
{
	Device* h = (Device*)handle;

	if (h == NULL || if_num <= 0 || if_num >= max_num_of_interfaces)
		return -1;
	if (h->interface_handles[if_num] == NULL)
		return -2;

	WinUsb_Free(h->interface_handles[if_num]);
	h->interface_handles[if_num] = NULL;
	return SUCCESS;
}

int get_serial_number(void* handle, char* buf)
{
	if (handle == NULL)
		return -1;

	Device* h = (Device*)handle;

	BOOL result;
	USB_DEVICE_DESCRIPTOR device_desc;
	ULONG length_received;
	result = WinUsb_GetDescriptor(h->interface_handles[0], USB_DEVICE_DESCRIPTOR_TYPE, 0, 0, (PBYTE)&device_desc, sizeof(device_desc), &length_received);
	if (FALSE == result || length_received != sizeof(device_desc)) {
		return -2;
	}
	unsigned char tmp[256];
	result = WinUsb_GetDescriptor(h->interface_handles[0], USB_STRING_DESCRIPTOR_TYPE, device_desc.iSerialNumber, 0, tmp, sizeof(tmp), &length_received);
	if (result == FALSE)
		return -3;

	/*
	*  The first byte of the STRING descriptor is the descriptor length,
	*  the second byte is the descriptor type (= 3 for a STRING descriptor),
	*  the actual string starts at offset 2 and is UTF-16 formatted.
	*/
	unsigned int j = 0;
	for (unsigned int i = 2; i < length_received; i += 2) {
		buf[j++] = tmp[i];
		//if (j >= sz - 1)
		//	break;
	}
	buf[j] = '\0';

	return j + 1;
}

bool reset_ep(void* handle, uint8_t ep)
{
	Device* h = (Device*)handle;

	BOOL bResult1 = WinUsb_ResetPipe(h->interface_handles[0], ep);
	BOOL bResult2 = WinUsb_ResetPipe(h->interface_handles[0], 0x80 | ep);
	return (bResult1 & bResult2);
}


/*
* GPIO functions
*/

int gpio(void* handle, enum operation_type op_type, char port, uint8_t pin)
{
	Device* h = (Device*)handle;

	if (h == NULL || h->interface_handles[0] == NULL)
		return -1;

	gpio_request_t request;

	request.operation = op_type;
	request.pin = pin;
	request.port = port;

	ULONG transferred = 0;
	BOOL bResult = WinUsb_WritePipe(h->interface_handles[0], gpio_pipe_id, (UCHAR*)&request, sizeof(request), &transferred, NULL);
	if (bResult != TRUE) {
		reset_ep(h, 2);
		return -2;
	}

	return transferred;
}

int gpio_config(void* handle, char port, uint8_t pin, uint8_t direction, uint8_t type, uint8_t pull)
{
	Device* h = (Device*)handle;

	if (h == NULL || h->interface_handles[0] == NULL)
		return -1;

	gpio_request_t request;

	request.operation = GPIO_CONFIG;
	request.pin = pin;
	request.port = port;
	request.direction = direction;
	request.type = type;
	request.pull = pull;

	ULONG transferred = 0;
	BOOL bResult = WinUsb_WritePipe(h->interface_handles[0], gpio_pipe_id, (UCHAR*)&request, sizeof(request), &transferred, NULL);
	if (bResult != TRUE) {
		reset_ep(h, 2);
		return -2;
	}

	return transferred;
}

int gpio_set(void* handle, char port, uint8_t pin)
{
	return gpio(handle, GPIO_SET, port, pin);
}

int gpio_clear(void* handle, char port, uint8_t pin)
{
	return gpio(handle, GPIO_CLEAR, port, pin);
}

int gpio_get(void* handle, char port, uint8_t pin, uint8_t* value)
{
	Device* h = (Device*)handle;

	if (h == NULL || h->interface_handles[0] == NULL)
		return -1;

	gpio_request_t request;

	request.operation = GPIO_GET;
	request.pin = pin;
	request.port = port;

	ULONG transferred = 0;
	BOOL bResult = WinUsb_WritePipe(h->interface_handles[0], gpio_pipe_id, (UCHAR*)&request, sizeof(request), &transferred, NULL);
	if (bResult != TRUE) {
		reset_ep(h, 2);
		return -2;
	}

	int v;
	bResult = WinUsb_ReadPipe(h->interface_handles[0], gpio_pipe_id | 0x80, (UCHAR*)&v, sizeof(v), &transferred, NULL);
	if (bResult != TRUE) {
		reset_ep(h, 2);
		return -3;
	}

	*value = (uint8_t)v;

	return 0;
}

