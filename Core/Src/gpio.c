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


#include "gpio.h"

gpio_request_t gpio_request;

/*
 * gpio_op_completed can only one of the following three values:
 * -1 : operation completed but failed
 *  0 : operation in progress
 *  1 : operation completed successfully
 */
int gpio_op_completed;

static GPIO_TypeDef* gpio_port(char port)
{
	switch(port) {
	case 'a':
	case 'A':
		return GPIOA;
	case 'b':
	case 'B':
		return GPIOB;
	case 'c':
	case 'C':
		return GPIOC;
	case 'd':
	case 'D':
		return GPIOD;
	case 'e':
	case 'E':
		return GPIOE;
	case 'f':
	case 'F':
		return GPIOF;
	case 'g':
	case 'G':
		return GPIOG;
	case 'h':
	case 'H':
		return GPIOH;
	default:
		return NULL;
	}
}

static uint32_t gpio_pin(uint8_t pin)
{
	switch(pin) {
	case 0:
		return LL_GPIO_PIN_0;
	case 1:
		return LL_GPIO_PIN_1;
	case 2:
		return LL_GPIO_PIN_2;
	case 3:
		return LL_GPIO_PIN_3;
	case 4:
		return LL_GPIO_PIN_4;
	case 5:
		return LL_GPIO_PIN_5;
	case 6:
		return LL_GPIO_PIN_6;
	case 7:
		return LL_GPIO_PIN_7;
	case 8:
		return LL_GPIO_PIN_8;
	case 9:
		return LL_GPIO_PIN_9;
	case 10:
		return LL_GPIO_PIN_10;
	case 11:
		return LL_GPIO_PIN_11;
	case 12:
		return LL_GPIO_PIN_12;
	case 13:
		return LL_GPIO_PIN_13;
	case 14:
		return LL_GPIO_PIN_14;
	case 15:
		return LL_GPIO_PIN_15;
	default:
		return 0;
	}
}

static uint32_t gpio_type(uint8_t type)
{
	switch(type) {
	case GPIO_PUSHPULL:
		return LL_GPIO_OUTPUT_PUSHPULL;
	case GPIO_OPENDRAIN:
		return LL_GPIO_OUTPUT_OPENDRAIN;
	default:
		return 0xFFFFFFFF;
	}
}

static uint32_t gpio_pull(uint8_t pull)
{
	switch(pull) {
	case GPIO_PULL_NO:
		return LL_GPIO_PULL_NO;
	case GPIO_PULL_UP:
		return LL_GPIO_PULL_UP;
	case GPIO_PULL_DOWN:
		return LL_GPIO_PULL_DOWN;
	default:
		return 0xFFFFFFFF;
	}
}

static uint32_t gpio_dir(uint8_t direction)
{
	switch(direction) {
	case GPIO_INPUT:
		return LL_GPIO_MODE_INPUT;
	case GPIO_OUTPUT:
		return LL_GPIO_MODE_OUTPUT;
	default:
		return 0xFFFFFFFF;
	}
}

int gpio_set(char port, uint8_t pin)
{
	gpio_op_completed = 0;
	GPIO_TypeDef* gport = gpio_port(port);
	uint32_t gpin = gpio_pin(pin);

	if(gpin==0 || gport==NULL) {
		gpio_op_completed = -1;
		return ERROR_GPIO_PARAMETER;
	}

	if(LL_GPIO_GetPinMode(gport,gpin) == LL_GPIO_MODE_ANALOG) {
		gpio_op_completed = -1;
		return ERROR_GPIO_ALREADY_USED_ANALOG;
	}

	if(LL_GPIO_GetPinMode(gport,gpin)==LL_GPIO_MODE_ALTERNATE) {
		gpio_op_completed = -1;
		return ERROR_GPIO_ALREADY_USED_AF;
	}

	LL_GPIO_SetOutputPin(gport, gpin);
	gpio_op_completed = 1;
	return 0;
}

int gpio_clear(char port, uint8_t pin)
{
	gpio_op_completed = 0;
	GPIO_TypeDef* gport = gpio_port(port);
	uint32_t gpin = gpio_pin(pin);

	if(gpin==0 || gport==NULL) {
		gpio_op_completed = -1;
		return ERROR_GPIO_PARAMETER;
	}

	if(LL_GPIO_GetPinMode(gport,gpin) == LL_GPIO_MODE_ANALOG) {
		gpio_op_completed = -1;
		return ERROR_GPIO_ALREADY_USED_ANALOG;
	}

	if(LL_GPIO_GetPinMode(gport,gpin)==LL_GPIO_MODE_ALTERNATE) {
		gpio_op_completed = -1;
		return ERROR_GPIO_ALREADY_USED_AF;
	}

	LL_GPIO_ResetOutputPin(gport, gpin);
	gpio_op_completed = 1;
	return 0;
}

int gpio_get(char port, uint8_t pin)
{
	gpio_op_completed = 0;
	GPIO_TypeDef* gport = gpio_port(port);
	uint32_t gpin = gpio_pin(pin);

	if(gpin==0 || gport==NULL) {
		gpio_op_completed = -1;
		return ERROR_GPIO_PARAMETER;
	}

	if(LL_GPIO_GetPinMode(gport,gpin) == LL_GPIO_MODE_ANALOG) {
		gpio_op_completed = -1;
		return ERROR_GPIO_ALREADY_USED_ANALOG;
	}

	if(LL_GPIO_GetPinMode(gport,gpin)==LL_GPIO_MODE_ALTERNATE) {
		gpio_op_completed = -1;
		return ERROR_GPIO_ALREADY_USED_AF;
	}

	gpio_op_completed = 1;
	return LL_GPIO_IsInputPinSet(gport, gpin);
}

int gpio_config(char port, uint8_t pin, enum gpio_direction direction,  enum gpio_output_type type, enum gpio_pull pull)
{
	gpio_op_completed = 0;
	GPIO_TypeDef* gport = gpio_port(port);
	uint32_t gpin = gpio_pin(pin);
	uint32_t gtype = gpio_type(type);
	uint32_t gpull = gpio_pull(pull);
	uint32_t gdir = gpio_dir(direction);

	if(gpin==0 || gport==NULL) {
		gpio_op_completed = -1;
		return ERROR_GPIO_PARAMETER;
	}

	if(LL_GPIO_GetPinMode(gport,gpin)==LL_GPIO_MODE_ALTERNATE) {
		gpio_op_completed = -1;
		return ERROR_GPIO_ALREADY_USED_AF;
	}

	if(gdir != 0xFFFFFFFF)
		LL_GPIO_SetPinMode(gport,gpin,gdir);
	if(gtype != 0xFFFFFFFF)
		LL_GPIO_SetPinOutputType(gport,gpin,gtype);
	if(gpull != 0xFFFFFFFF)
		LL_GPIO_SetPinPull(gport,gpin,gpull);

	gpio_op_completed=1;

	return 0;
}

