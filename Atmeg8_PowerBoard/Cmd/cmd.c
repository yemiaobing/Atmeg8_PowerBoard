#include "cmd.h"
#include "../Bsp/bsp_uart.h"
#include <stdlib.h>
#include <string.h>
#include "../Power_management/power_management.h"

static void get_all_bat_info_cmd_exec(void)
{
	uint8_t buf[MSG_BAT_INFO_LENGTH] = {0};
	uint8_t i = 0;
	
	/* head */
	buf[i++] = 0xAA;
	buf[i++] = 0xFF;
	buf[i++] = 0x01;
	buf[i++] = 0x10;
	/* content */
	buf[i++] = battery_get_cur_bat_voltage();
	buf[i++] = battery_get_cur_bat_voltage() >> 8;
	buf[i++] = battery_get_cur_charge_voltage();
	buf[i++] = battery_get_cur_charge_voltage() >> 8;
	buf[i++] = battery_get_full_voltage();
	buf[i++] = battery_get_full_voltage() >> 8;
	buf[i++] = battery_get_low_voltage();
	buf[i++] = battery_get_low_voltage() >> 8;
	buf[i++] = battery_get_percentage();
	/*reserver 7 bytes*/
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0;
	/* tail */
	buf[i++] = 0x55;
	buf[i++] = 0xFF;
	
	uart_send_buf(buf, MSG_BAT_INFO_LENGTH);
}

static void get_bat_power_cmd_exec(void)
{
	uint8_t buf[MSG_BAT_POWER_LENGTH] = {0};
	uint8_t i = 0;
	
	/* head */
	buf[i++] = 0xAA;
	buf[i++] = 0xFF;
	buf[i++] = 0x04;
	buf[i++] = 0x10;
	/* content */
	buf[i++] = battery_get_percentage();
	/* tail */
	buf[i++] = 0x55;
	buf[i++] = 0xFF;
	
	uart_send_buf(buf, MSG_BAT_POWER_LENGTH);
}

void set_bat_para_cmd_exec(uint8_t* cmd_buf)
{
	Bat_managent_t bat_m;
	uint8_t buf[MSG_SET_BAT_REPLY_LENGTH] = {0};
	uint8_t i = 0;
	
	bat_m.bat_full_vol = cmd_buf[4];
	bat_m.bat_full_vol |= cmd_buf[5] << 8;
	bat_m.bat_low_vol = cmd_buf[6];
	bat_m.bat_low_vol |= cmd_buf[7] << 8;
	bat_m.internal_refer_vol = cmd_buf[8];
	bat_m.internal_refer_vol |= cmd_buf[9] << 8;
	bat_m.internal_vol_div_resistor1 = cmd_buf[10];
	bat_m.internal_vol_div_resistor2 = cmd_buf[11];
	
	/* head */
	buf[i++] = 0xAA;
	buf[i++] = 0xFF;
	buf[i++] = 0x02;
	buf[i++] = 0x10;
	/* tail */
	buf[i++] = 0x55;
	buf[i++] = 0xFF;
	
	battery_set_bat_managent(&bat_m);
	uart_send_buf(buf, MSG_SET_BAT_REPLY_LENGTH);
}

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
			get_all_bat_info_cmd_exec();
			break;
		}
		
		case SET_BAT_PARA_CMD:
		{
			set_bat_para_cmd_exec(cmd_string);
			break;
		}
		
		#if 0
		case CONTROL_LED_CMD:
		{
			break;
		}
		#endif
		
		case GET_BAT_POWER_CMD:
		{
			get_bat_power_cmd_exec();
			break;
		}
		
		default:
		{
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