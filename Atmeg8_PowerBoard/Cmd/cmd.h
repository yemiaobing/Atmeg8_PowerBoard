#ifndef CMD_H_
#define CMD_H_

#include "../Bsp/eeprom.h"

#define CMD_HEAD1 0xAA
#define CMD_HEAD2 0xFF

#define CMD_TAIL1 0x55
#define CMD_TAIL2 0xFF

#define MAX_CMD_LENGTH  50

#define GET_ALL_BAT_INFO_CMD 0x1001
#define SET_BAT_PARA_CMD	0x1002
#define CONTROL_LED_CMD		0x1003
#define GET_BAT_POWER_CMD	0x1004

#define MSG_BAT_INFO_LENGTH	22
#define MSG_BAT_POWER_LENGTH 7
#define MSG_SET_BAT_REPLY_LENGTH	6

typedef enum
{
	WAIT_HEAD1 = 0,
	WAIT_HEAD2,
	WAIT_CMD,
	WAIT_CONTENT,
	WAIT_TAIL1,
	WAIT_TAIL2
}CMD_PARSE_STATE_E;

extern void cmd_receive(void);

#endif /* CMD_H_ */