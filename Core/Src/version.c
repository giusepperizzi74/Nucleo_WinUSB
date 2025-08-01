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

#include "version.h"

const uint8_t FW_ver_major = 1;
const uint8_t FW_ver_minor = 0;
const uint16_t FW_ver_build = 0;

// The .verinfo section is linked at the end of the binary image so that the FW revision numbers 
// can be fetched by an automatic tool

const version_info_t version_info __attribute__((section(".ver_info"),aligned(4))) = {.extra_string="Generic customer", .version={FW_ver_major,FW_ver_minor,FW_ver_build>>8,FW_ver_build & 0xFF}};
