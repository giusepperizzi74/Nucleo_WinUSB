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

#ifndef INC_CLI_H_
#define INC_CLI_H_

#include "stm32h5xx_ll_usart.h"
#include "stdio.h"

#define ESC1			'\e'
#define ESC2			'['
#define ARROW_UP		'A'
#define ARROW_DOWN		'B'
#define CL_MAX_TOKENS 		1024
#define CL_MAX_TOKEN_LENGTH 10
#define CL_MAX_LENGTH 		(CL_MAX_TOKEN_LENGTH*CL_MAX_TOKENS)

#ifdef USE_CLI
#define DPRINT(...) (fprintf(stderr,__VA_ARGS__))
#define DSCAN(...) (dscanf(__VA_ARGS__))
#define DGETCL(str) (dgetcl(str))
#define STRPRINT(...) (str_print(__VA_ARGS__))
#else
#define DPRINT(...) __NOP()
#define DSCAN(...) __NOP()
#define DGETCL(str) __NOP()
#define STRPRINT(...) __NOP()
#endif
//void dprint(const char* fmt, ...);
int dscanf(const char* fmt, ...);
int dgetcl(char* cline);
extern const char arrow_up[];
extern const char arrow_down[];
void str_print(const char* fmt, ...);
void cli_print();
#define TRACE_LENGTH 4096 	// must be a power of 2
extern char trace[TRACE_LENGTH];

#endif /* INC_CLI_H_ */
