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

#include "cli.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define USART_DEBUG USART3
#define USART_BUFSIZE 4096

static char cli_ibuffer[USART_BUFSIZE];
char trace[TRACE_LENGTH];
static int w_idx, r_idx;
const char arrow_up[] = {'\e','[','A','\0'};
const char arrow_down[] = {'\e','[','B','\0'};

void trace_print()
{
	while(r_idx != w_idx) {
		putchar(trace[r_idx++]);
		r_idx &= (TRACE_LENGTH-1);
	}
}

void str_print(const char* fmt, ...)
{
	char tmp[512];
    va_list args;
    va_start(args,fmt);
    vsprintf(tmp,fmt,args);
    for(int i=0;tmp[i]!=0;i++) {
    	trace[w_idx++] = tmp[i];
    	w_idx &= (TRACE_LENGTH-1);
    }
    va_end(args);
}

int __io_putchar(int c)
{
	while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
	if(c=='\n')
	{
		LL_USART_TransmitData8(USART_DEBUG,'\r');
		while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
	}
	LL_USART_TransmitData8(USART_DEBUG,c);

	while(!LL_USART_IsActiveFlag_TC(USART_DEBUG)) {}

	return c;
}

static void clear_cl(int num)
{
	for(int i=0;i<num;i++) {
        while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
        LL_USART_TransmitData8(USART_DEBUG,'\b');
	}
	for(int i=0;i<num;i++) {
        while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
        LL_USART_TransmitData8(USART_DEBUG,' ');
	}
	for(int i=0;i<num;i++) {
        while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
        LL_USART_TransmitData8(USART_DEBUG,'\b');
	}
}

static void fill_ibuffer(int si)
{
    char c;
    char esc[3];

    for(int i=si;i<USART_BUFSIZE;)
    {
        while( !LL_USART_IsActiveFlag_RXNE_RXFNE(USART_DEBUG)) {
        	trace_print();
        }

        c = LL_USART_ReceiveData8(USART_DEBUG);
        if(c==arrow_up[0]) {	// handle escape sequence
        	while( !LL_USART_IsActiveFlag_RXNE_RXFNE(USART_DEBUG)) {}
        	esc[0]=LL_USART_ReceiveData8(USART_DEBUG);
        	while( !LL_USART_IsActiveFlag_RXNE_RXFNE(USART_DEBUG)) {}
        	esc[1]=LL_USART_ReceiveData8(USART_DEBUG);
        	if(esc[0]==arrow_up[1]) {
        		switch(esc[1]) {
        		case 'A':
        			strcpy(cli_ibuffer,arrow_up);
        			clear_cl(i);
        			return;
        		case 'B':
        			strcpy(cli_ibuffer,arrow_down);
        			clear_cl(i);
        			return;
        		default:
        			continue;
        		}
        	}
        }
        if(c=='\r') // Enter
        {
            cli_ibuffer[i]='\0'; // add string-end character
            // echo carriage return
            while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
            LL_USART_TransmitData8(USART_DEBUG,'\n');
            while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
            LL_USART_TransmitData8(USART_DEBUG,'\r');
            break;
        }
        switch(c)
        {
        case 0x7F:
        case 0x08:  // handle backspace
            if(i>0) {
                i--;
                // echo
                while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
                LL_USART_TransmitData8(USART_DEBUG,'\b');
                while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
                LL_USART_TransmitData8(USART_DEBUG,' ');
                while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
                LL_USART_TransmitData8(USART_DEBUG,'\b');
            }
            break;
        default:
            cli_ibuffer[i]=c;
            i++;
			while(!LL_USART_IsActiveFlag_TXE_TXFNF(USART_DEBUG)) {}
			LL_USART_TransmitData8(USART_DEBUG,c);
            break;
        }
    }
}

int dscanf(const char* fmt, ...)
{
    //char c;
    int rc;

    fill_ibuffer(0);

    va_list args;
    va_start(args,fmt);
    rc = vsscanf(cli_ibuffer, fmt, args);
    va_end(args);

    return rc;
}

#define HISTORY_LENGTH 8
char cl_history[HISTORY_LENGTH][CL_MAX_LENGTH];
uint16_t hist_last;
uint16_t hist_curr;

int dgetcl(char* cline)
{
	cli_ibuffer[0]='\0';
	DPRINT(">");
	while(1) {
		fill_ibuffer(strlen(cli_ibuffer));
		if(strcmp(cli_ibuffer,arrow_up)==0) {
			strcpy(cli_ibuffer,cl_history[hist_curr]);
			DPRINT(cli_ibuffer);
			hist_curr--;
			if(hist_curr >= HISTORY_LENGTH) hist_curr = HISTORY_LENGTH-1;
		}
		else if(strcmp(cli_ibuffer,arrow_down)==0) {
			strcpy(cli_ibuffer,cl_history[hist_curr]);
			DPRINT(cli_ibuffer);
			hist_curr = (hist_curr+1) % HISTORY_LENGTH;
		}
		else {
			strcpy(cline,cli_ibuffer);
			if(cline[0] != '\0') {
				strcpy(cl_history[hist_last],cline);
				hist_curr = hist_last;
				hist_last = (hist_last+1) % HISTORY_LENGTH;
			}
			break;
		}
	}
	return strlen(cline);
}
