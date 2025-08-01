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

#include "main.h"
#include "cli.h"
#include "mcu_init.h"
#include "version.h"
#include "gpio.h"
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>

__IO uint8_t ubButtonPress = 0;

void led(int on_mask, int off_mask)
{
	if(on_mask & LED_GREEN)
		LL_GPIO_SetOutputPin(LED1_GPIO_Port, LED1_Pin);
	if(on_mask & LED_YELLOW)
		LL_GPIO_SetOutputPin(LED2_GPIO_Port, LED2_Pin);
	if(on_mask & LED_RED)
		LL_GPIO_SetOutputPin(LED3_GPIO_Port, LED3_Pin);
	if(off_mask & LED_GREEN)
		LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin);
	if(off_mask & LED_YELLOW)
		LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);
	if(off_mask & LED_RED)
		LL_GPIO_ResetOutputPin(LED3_GPIO_Port, LED3_Pin);
}

static void print_help()
{
	DPRINT("\n");
	DPRINT("help                                                 -- print this menu.\n");
	DPRINT("gpio [-d 0|1] [-p 0|1|2] [-t 0|1] set|clear|get e?   -- Get/Set/Clear a gpio, e.g., g4.\n");
	DPRINT("                                                        Options:\n");
	DPRINT("                                                        -d 0|1   -> gpio direction: input(0), output(1)\n");
	DPRINT("                                                        -p 0|1|2 -> pull-ups: none(0), up(1), down(2)\n");
	DPRINT("                                                        -t 0|1   -> output type: pushpull(0), opendrain(1)\n");
	DPRINT("\n");
}

static int tokenise(char* str, char** tokens)
{
	int count=0;
	int i=0;
	size_t length = strlen(str)+1;
	while(1) {
		if(str[i]=='\0') {
			tokens[count]=NULL;
			return count;
		}
		else if(str[i] < '!' || str[i]> 126) {
			i++;
		}
		else {
			tokens[count] = &str[i];
			count++;
			while(str[i] > '!' && str[i]<127) i++;
			str[i++] = '\0';
			if(i>=length) {
				tokens[count]=NULL;
				return count;
			}
		}
	}
	return count;
}

static int is_number(const char* str)
{
	if(str==NULL)
		return 0;

	if(str[0]=='0' && (str[1]=='x' || str[1]=='X') ) {
		for(int i=2; i<strlen(str); i++)
			if( !isalnum((int)str[i]))
				return 0;
	}
	else {
		for(int i=0;i<strlen(str);i++)
			if(!isdigit((int)str[i]))
				return 0;
	}
	return 1;
}

static int str_to_int(const char* str)
{
	char* end_ptr;
	return strtol(str,&end_ptr,0);
}

/*
 * GPIO functions
 */
static int gpio(char** tokens)
{
	char port = 0;
	uint8_t pin = 0xFF;
	uint8_t type = 0xFF;
	uint8_t pull = 0xFF;
	uint8_t direction = 0xFF;
	int op_type = 0;

	int i;
	for(i=1;tokens[i]!=NULL;i++) {
		if(strcmp(tokens[i],"-t")==0) {
			i++;
			if(is_number(tokens[i])) {
				type=str_to_int(tokens[i]);
			}
			else
				break;
		}
		else if(strcmp(tokens[i],"-p")==0) {
			i++;
			if(is_number(tokens[i])) {
				pull=str_to_int(tokens[i]);
			}
			else
				break;
		}
		else if(strcmp(tokens[i],"-d")==0) {
			i++;
			if(is_number(tokens[i])) {
				direction=str_to_int(tokens[i]);
			}
			else
				break;
		}
		else if(strcmp(tokens[i],"set")==0) {
			op_type=1;
		}
		else if(strcmp(tokens[i],"clear")==0) {
			op_type=2;
		}
		else if(strcmp(tokens[i],"get")==0) {
			op_type=3;
		}
		else {
			port = tokens[i][0];
			if(is_number(&tokens[i][1]))
				pin = str_to_int(&tokens[i][1]);
			else
				break;
		}
	}
	if(tokens[i]!= NULL) {
		DPRINT("The command is ill-formatted.\n");
		return 0;
	}

	if(type != 0xFF || pull != 0xFF || direction != 0xFF)
		if(gpio_config(port, pin, direction, type, pull) < 0)
			return -1;

	if(op_type==1)
		return gpio_set(port,pin);
	if(op_type==2)
		return gpio_clear(port,pin);
	if(op_type==3) {
		int p = gpio_get(port,pin);
		if(p>=0) {
			DPRINT("%d\n",p);
			return 0;
		}
		return -1;
	}

	return -1;
}

int main(void)
{
	mcu_init();

	DPRINT("\n\n** NUCLEO-H563ZI I3C Controller v%d.%d.%d, %s %s **\n",FW_ver_major,FW_ver_minor,FW_ver_build,__DATE__,__TIME__);
	print_help();
	char cmd_line[CL_MAX_LENGTH];
	char* tokens[CL_MAX_TOKENS];
	while(1) {

		DGETCL(cmd_line);
		tokenise(cmd_line,tokens);
		if(tokens[0]==NULL)
			continue;
		if(strcmp(tokens[0],"help")==0)
			print_help();

		/* GPIO commands */
		else if(strcmp(tokens[0],"gpio")==0) {
			if(gpio(tokens) < 0)
				DPRINT("Error\n");
		}

		else
			DPRINT("The command is ill-formatted\n");
	}
}

