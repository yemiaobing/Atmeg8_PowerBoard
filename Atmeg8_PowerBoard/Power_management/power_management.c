#include "power_management.h"
#include "../Bsp/bsp.h"
#include "../Bsp/bsp_uart.h"
#include "../Bsp/timer.h"
#include <avr/interrupt.h>

void power_pin_init(void)
{
	DDRC &= (1 << DDD3); //pin1 PD3 设置为输入
	DDRD |= 1 << DDD4;	//pin2 PD4 设置为输出
	DDRC |= 1 << DDC5;  //pin28 PC5 设置为输出
	DDRD &= (1 << DDD2); //pin32 PD2 设置为输入
	
	PIN2_OUTPUT_HIGH();
}

static uint8_t key_scan(void)
{
	static uint8_t has_detect_key_low = 0;
	static uint32_t first_detect_key_low_time = 0;
	
	if (PIN1_INPUT_VALUE == 0)
	{
		if (has_detect_key_low == 1)
		{
			if (millis() - first_detect_key_low_time >= LONG_KEY_TIME)
			{
				has_detect_key_low = 0;
				first_detect_key_low_time = 0;
				
				return DETECT_KEY_LONG_PRESSED;
			}
		}
		else
		{
			first_detect_key_low_time = millis();
			has_detect_key_low = 1;
		}
	}
	else
	{
		if (has_detect_key_low == 1)
		{
			uint32_t detect_high_time = millis();			
			
			if (detect_high_time - first_detect_key_low_time >= SHAKING_TIME)
			{
				has_detect_key_low = 0;
				first_detect_key_low_time = 0;
				
				return DETECT_KEY_PRESSED;	
			}
			
			has_detect_key_low = 0;
			first_detect_key_low_time = 0;
		}
	}
	
	return DETECT_NO_KEY_PRESSED;
}

void device_state_manage(void)
{	
	static uint32_t pre_time = 0;
	static uint8_t device_state = POWER_ON_DETECT;
	uint8_t key_value = DETECT_NO_KEY_PRESSED;
	
	switch(device_state)
	{
		case POWER_ON_DETECT:
		{
			PIN2_OUTPUT_HIGH();
			pre_time = millis();
			device_state = POWER_ON_WAIT_2_SECONDS;
			break;
		}
		
		case POWER_ON_WAIT_2_SECONDS:
		{
			/* 开机等待2s后 */
			if (millis() - pre_time >= 2000)
			{
				PIN28_OUTPUT_HIGH();
				pre_time = millis();
				device_state = POWER_ON_OUTPUT_20MS_PULSE;
			}
			break;
		}
		
		case POWER_ON_OUTPUT_20MS_PULSE:
		{
			if (millis() - pre_time >= 20)
			{
				PIN28_OUTPUT_LOW();
				device_state = NORMAL_WORK;				
			}
			break;
		}
		
		case NORMAL_WORK:
		{
			key_value = key_scan();
			if (key_value != DETECT_NO_KEY_PRESSED)
			{
				if (key_value == DETECT_KEY_PRESSED)
				{
					device_state = POWER_OFF_DETECT;
				}
				else //DETECT_KEY_LONG_PRESSED
				{
					device_state = FORCE_SHUTDOWN_DETECT;
				}
			}
			break;	
		}
		
		case POWER_OFF_DETECT:
		{
			PIN28_OUTPUT_HIGH();
			pre_time = millis();
			device_state = POWER_OFF_OUTPUT_20MS_PULSE;
			
			break;
		}
		
		case POWER_OFF_OUTPUT_20MS_PULSE:
		{
			if (millis() - pre_time >= 20)
			{
				PIN28_OUTPUT_LOW();
				device_state = POWER_OFF_WAIT_MAIN_BOARD_OFF;
			}
			
			break;
		}
		
		case POWER_OFF_WAIT_MAIN_BOARD_OFF:
		{
			/* 检测PIN32脚为高电平 */
			if (PIN32_INPUT_VALUE == 1)
			{
				PIN2_OUTPUT_LOW();
			}
			break;
		}
		
		case FORCE_SHUTDOWN_DETECT:
		{
			PIN28_OUTPUT_HIGH();
			pre_time = millis();
			device_state = FORCE_SHUTDOWN_OUTPUT_6S_PULSE;
			
			break;
		}
		
		case FORCE_SHUTDOWN_OUTPUT_6S_PULSE:
		{
			/* PIN28 给高脉冲6s*/
			if (millis() - pre_time >= 6000)
			{
				PIN28_OUTPUT_LOW();
				PIN2_OUTPUT_LOW();
			}
			
			break;
		}
		
		default:
			break;	
	}
}

