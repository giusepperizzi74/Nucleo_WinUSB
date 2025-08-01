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

#ifndef _VERSION_H
#define _VERSION_H

#include <stdint.h>

extern const uint8_t FW_ver_major;
extern const uint8_t FW_ver_minor;
extern const uint16_t FW_ver_build;

#define EXTRA_STRING_LENGTH 32
typedef struct {
    const char extra_string[EXTRA_STRING_LENGTH+4]; // EXTRA_STRING_LENGTH is 32 bytes in the CMIS, here there is +4 to make room for string terminating character while maintaining the size a multiple of 4.
    uint8_t version[4];
    uint32_t crc32;  // to be filled by CRC_Calculator.exe post-link
} version_info_t;
extern const version_info_t version_info;

#endif // _VERSION_H
