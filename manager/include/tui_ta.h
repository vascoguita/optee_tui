#ifndef TA_TUI_H
#define TA_TUI_H

#define TA_TUI_UUID \
	{ 0xa6a1c947, 0x92fe, 0x41e4, \
		{ 0x9b, 0x6b, 0x3a, 0x83, 0xef, 0xd4, 0x33, 0x4f} }

#define TA_TUI_CMD_SETUP    0
#define TA_TUI_CMD_PRINT    1
#define TA_TUI_CMD_INPUT	2

#define PORT_MIN                0
#define PORT_MAX                65535
#define MIN_IPV4_SIZE           8       // 0.0.0.0\0
#define MAX_IPV4_SIZE           16      // 255.255.255.255\0

#endif