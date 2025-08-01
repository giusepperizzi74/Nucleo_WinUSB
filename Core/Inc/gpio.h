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

#ifndef _GPIO_H_
#define _GPIO_H_

#include "stm32h5xx_ll_gpio.h"

#define ERROR_NONE						0
#define ERROR_GPIO_PARAMETER			-1
#define ERROR_GPIO_ALREADY_USED_AF		-2
#define ERROR_GPIO_ALREADY_USED_ANALOG	-3
#define ERROR_COUNT						-4

enum operation_type {

	/* gpio */
	GPIO_SET = 0x0200,
	GPIO_CLEAR,
	GPIO_GET,
	GPIO_CONFIG,

	NO_OP = 0xFFFF
};

enum gpio_direction {
	GPIO_INPUT = 0,
	GPIO_OUTPUT
};

enum gpio_output_type {
	GPIO_PUSHPULL = 0,
	GPIO_OPENDRAIN
};

enum gpio_pull {
	GPIO_PULL_NO = 0,
	GPIO_PULL_UP,
	GPIO_PULL_DOWN
};

typedef struct {
	uint32_t operation;
	uint8_t port;
	uint8_t pin;
	uint8_t direction;
	uint8_t type;
	uint8_t pull;
} gpio_request_t;

int gpio_set(char port, uint8_t pin);
int gpio_clear(char port, uint8_t pin);
int gpio_get(char port, uint8_t pin);
int gpio_config(char port, uint8_t pin, enum gpio_direction direction, enum gpio_output_type type, enum gpio_pull pull );

extern int gpio_op_completed;
extern gpio_request_t gpio_request;

#endif /* _GPIO_H_ */
