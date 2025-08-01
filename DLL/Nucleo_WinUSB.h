#pragma once

/** @cond IGNORE */
#ifdef NUCLEO_WINUSB_EXPORTS
#define NUCLEO_WINUSB_API __declspec(dllexport)
#else
#define NUCLEO_WINUSB_API __declspec(dllimport)
#endif

#include <stdint.h>
/** @endcond */

/// @brief Call this function repeatedly to get pointers to descriptors of all connected devices.
/// @param[in] desc Pointer returned from the previous call. Use NULL to get the descriptor of the first device.
/// @returns Pointer to the descriptor of the next device. NULL if there are no other devices.
extern "C" NUCLEO_WINUSB_API char* get_next(char* desc);

/// @brief  Gets a USB device handle from its descriptor. 
/// 
/// Use this function to get a handle to a specific device from its descriptor, which can be obtained with get_next(). The device default interface is opened.
/// @param[in] desc Pointer to the device descriptor.
/// @returns Handle to the device.
extern "C" NUCLEO_WINUSB_API void* open(char* desc);

/// @brief Use this function to release the device's handle.
/// @param[in] handle Handle obtained from open().
/// @returns A negative value if the operation has failed.
extern "C" NUCLEO_WINUSB_API int close(void* handle);

/// @brief Use this function to retrieve the serial number of the Nucleo_WinUSB from its handle.
/// @param[in] handle Handle obtained from open().
/// @param[out] buf Pointer to a C-style string that will contain the adapter serial number. It must be at least 25-character long.
/// @returns The string length if successful, a negative value if the operation has failed.
extern "C" NUCLEO_WINUSB_API int get_serial_number(void* handle, char* buf);

/// @brief This function configures the output type (0-pushpull, 1-opendrain) and the pull-up/down resistors (0-none, 1-up, 2-down) of a GPIO pin.
/// @param[in] handle Handle obtained from open().
/// @param[in] port GPIO port. Must be a letter from 'a' to 'h'.
/// @param[in] pin GPIO pin. Must be a number from 0 to 15.
/// @param[in] direction. Must be 0 for input, 1 for output.
/// @param[in] type Output type. Must be 0 for pushpull, 1 for opendrain.
/// @param[in] pull Pullup/down resistors. Must be 0 for none, 1 for pullup and 2 for pulldown.
/// @returns int variable. Holds the operation result [success(>=0), fail(<0)]
extern "C" NUCLEO_WINUSB_API int gpio_config(void* handle, char port, uint8_t pin, uint8_t direction, uint8_t type, uint8_t pull);

/// @brief This function sets a GPIO output value.
/// @param[in] handle Handle obtained from open().
/// @param[in] port GPIO port. Must be a letter from 'a' to 'h'.
/// @param[in] pin GPIO pin. Must be a number from 0 to 15.
/// @returns int variable. Holds the operation result [success(>=0), fail(<0)]
extern "C" NUCLEO_WINUSB_API int gpio_set(void* handle, char port, uint8_t pin);

/// @brief This function clears a GPIO output value.
/// @param[in] handle Handle obtained from open().
/// @param[in] port GPIO port. Must be a letter from 'a' to 'h'.
/// @param[in] pin GPIO pin. Must be a number from 0 to 15.
/// @returns int variable. Holds the operation result [success(>=0), fail(<0)]
extern "C" NUCLEO_WINUSB_API int gpio_clear(void* handle, char port, uint8_t pin);

/// @brief This function gets a GPIO input value.
/// @param[in] handle Handle obtained from open().
/// @param[in] port GPIO port. Must be a letter from 'a' to 'h'.
/// @param[in] pin GPIO pin. Must be a number from 0 to 15.
/// @param[out] value GPIO value. This parameter will contain the GPIO value (either 0 or 1) if the operation is successful.
/// @returns int variable. Holds the operation result [success(>=0), fail(<0)]
extern "C" NUCLEO_WINUSB_API int gpio_get(void* handle, char port, uint8_t pin, uint8_t* value);

