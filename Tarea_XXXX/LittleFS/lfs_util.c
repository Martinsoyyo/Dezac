/*
 * lfs util functions
 *
 * Copyright (c) 2022, The littlefs authors.
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "lfs_util.h"


void lfs_debug_print(const char* file, int line, const char* fmt, ...) {
	char buffer[512];
	va_list args;
	va_start(args, fmt);
	int res = vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (res < 0 || res >= sizeof(buffer)) {
		printf("Error: Cadena de formato demasiado larga en lfs_error_print\n");
	}
	else {
		printf("%s:%d:debug: %s\n", file, line, buffer);
	}
}


void lfs_warn_print(const char* file, int line, const char* fmt, ...) {
	char buffer[512];
	va_list args;
	va_start(args, fmt);
	int res = vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (res < 0 || res >= sizeof(buffer)) {
		printf("Error: Cadena de formato demasiado larga en lfs_error_print\n");
	}
	else {
		printf("%s:%d:warning: %s\n", file, line, buffer);
	}
}

void lfs_error_print(const char* file, int line, const char* fmt, ...) {
	char buffer[512];
	va_list args;
	va_start(args, fmt);
	int res = vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	if (res < 0 || res >= sizeof(buffer)) {
		printf("Error: Cadena de formato demasiado larga en lfs_error_print\n");
	}
	else {
		printf("%s:%d:error: %s\n", file, line, buffer);
	}
}

 // Only compile if user does not provide custom config
#ifndef LFS_CONFIG


// If user provides their own CRC impl we don't need this
#ifndef LFS_CRC
// Software CRC implementation with small lookup table
uint32_t lfs_crc(uint32_t crc, const void* buffer, size_t size) {
	static const uint32_t rtable[16] = {
		0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
		0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
		0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
		0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c,
	};

	const uint8_t* data = buffer;

	for (size_t i = 0; i < size; i++) {
		crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 0)) & 0xf];
		crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 4)) & 0xf];
	}

	return crc;
}
#endif


#endif
