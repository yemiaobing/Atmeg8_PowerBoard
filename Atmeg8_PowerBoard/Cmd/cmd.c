#include "cmd.h"
#include "../Bsp/bsp_uart.h"
#include <stdlib.h>
#include <string.h>
#include "../Bsp/eeprom.h"

static uint8_t cmd_get_content_length(uint16_t cmd)
{
	switch (cmd)
	{
		case GET_ALL_BAT_INFO_CMD:
		{
			return 0;
		}
		
		case SET_BAT_PARA_CMD:
		{			
			return 16;
		}
		
		case CONTROL_LED_CMD:
		{
			return 1;	
		}
		
		case GET_BAT_POWER_CMD:
		{
			return 0;	
		}
		
		default:
			return 0;
	}
	
	return 0;
}

static int8_t cmd_parse(uint8_t *cmd_string, uint8_t cmd_legnth, uint16_t cmd_type)
{
	switch (cmd_type)
	{
		case GET_ALL_BAT_INFO_CMD:
		{			
			//uart_send_byte(eeprom_read(0));
			//uart_send_buf("GET_ALL_BAT_INFO_CMD\n", strlen("GET_ALL_BAT_INFO_CMD"));
			break;
		}
		
		case SET_BAT_PARA_CMD:
		{
			//eeprom_write(0, 0x53);
			//uart_send_buf("SET_BAT_PARA_CMD\n", strlen("SET_BAT_PARA_CMD"));
			break;
		}
		
		case CONTROL_LED_CMD:
		{
			uart_send_buf("CONTROL_LED_CMD\n", strlen("CONTROL_LED_CMD"));
			break;
		}
		
		case GET_BAT_POWER_CMD:
		{
			uart_send_buf("GET_BAT_POWER_CMD\n", strlen("GET_BAT_POWER_CMD"));
			break;
		}
		
		default:
		{
			uart_send_buf("default\n", strlen("default"));
			return -1;
		}  
	}
	
	return 0;
}

void cmd_receive(void)
{
	static uint8_t cmd_parse_state = WAIT_HEAD1;
	static uint8_t cmd_buf[MAX_CMD_LENGTH] = {0};
	static uint8_t cmd_count = 0;
	static uint16_t cmd_type = 0;
	static uint8_t cmd_content_length = 0;
	uint8_t recv_byte = 0;
	if (uart_get_byte(&recv_byte) == 0)
	{
		switch (cmd_parse_state)
		{
			case WAIT_HEAD1:
			{
				memset(cmd_buf, 0x00, MAX_CMD_LENGTH);
				cmd_count = 0;
				cmd_content_length = 0;
				cmd_type = 0;
				if (recv_byte == CMD_HEAD1)
				{
					cmd_buf[cmd_count++] = recv_byte;
					cmd_parse_state = WAIT_HEAD2;
				}
				break;
			}
			
			case WAIT_HEAD2:
			{
				if (recv_byte == CMD_HEAD2)
				{
					cmd_buf[cmd_count++] = recv_byte;
					cmd_parse_state = WAIT_CMD;
				}
				else
				{
					cmd_parse_state = WAIT_HEAD1;
				}
				
				break;
			}
			
			case WAIT_CMD:
			{
				cmd_buf[cmd_count++] = recv_byte;
				if (cmd_count == 4)
				{
					cmd_type = cmd_buf[2] | (cmd_buf[3] << 8);
					cmd_content_length = cmd_get_content_length(cmd_type);
					if (cmd_content_length > 0)
					{
						cmd_parse_state = WAIT_CONTENT;
					}
					else
					{
						cmd_parse_state = WAIT_TAIL1;
					}
				}
				break;
			}
			
			case WAIT_CONTENT:
			{
				cmd_buf[cmd_count++] = recv_byte;
				if (cmd_count == (4 + cmd_content_length))
				{
					cmd_parse_state = WAIT_TAIL1;	
				}
				break;
			}
			
			case WAIT_TAIL1:
			{
				if (recv_byte == CMD_TAIL1)
				{
					cmd_buf[cmd_count++] = recv_byte;
					cmd_parse_state = WAIT_TAIL2;
				}
				else
				{
					cmd_parse_state = WAIT_HEAD1;	
				}
				break;
			}
			
			case WAIT_TAIL2:
			{
				if (recv_byte == CMD_TAIL2)
				{
					cmd_buf[cmd_count++] = recv_byte;
					cmd_parse_state = WAIT_HEAD1;
					cmd_parse(cmd_buf, cmd_count, cmd_type);
				}
				else
				{
					cmd_parse_state = WAIT_HEAD1;
				}
				break;
			}
		}
	}
}