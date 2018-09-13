#include "power_management.h"
#include "../Bsp/bsp.h"
#include "../Bsp/bsp_uart.h"
#include "../Bsp/timer.h"
#include <avr/interrupt.h>
#include "string.h"

static uint16_t g_cur_bat_voltage = 0;
static uint16_t g_cur_charge_voltage = 0;
static Bat_managent_t bat_managent;

void power_pin_init(void)
{
	DDRC &= (1 << DDD3); //pin1 PD3 设置为输入
	DDRD |= 1 << DDD4;	//pin2 PD4 设置为输出
	DDRC |= 1 << DDC5;  //pin28 PC5 设置为输出
	DDRD &= (1 << DDD2); //pin32 PD2 设置为输入	
}

void power_on(void)
{
	PIN2_OUTPUT_HIGH();
	_delay_ms(2000);
	PIN28_OUTPUT_HIGH();
	_delay_ms(100);
	PIN28_OUTPUT_LOW();
}

void key_scan(void)
{
	if (PIN1_INPUT_VALUE == 0)
	{
		PIN28_OUTPUT_HIGH();
	}
	else
	{
		PIN28_OUTPUT_LOW();
	}
	
	if (PIN32_INPUT_VALUE == 1)
	{
		/* 一检测到PIN32脚为高电平,PIN2就输出低电平,关机 */
		PIN2_OUTPUT_LOW();
	}
}

void battery_init(void)
{
	adc_init();
	
	eeprom_read_bytes(BAT_MANAGENT_EEPROM_ADDR, (uint8_t*)&bat_managent, sizeof(bat_managent));
	
	if (bat_managent.bat_full_vol == 0 || (bat_managent.bat_full_vol <= bat_managent.bat_low_vol))	//EEPROM保存的充满电压和放空不正常，使用默认值
	{
		bat_managent.bat_full_vol = DEFAULT_BATTERY_FULL_VOL;
		bat_managent.bat_low_vol = DEFAULT_BATTERY_LOW_VOL;
	}
}

void need_enter_power_off_status(void)
{
	int8_t check_debounce_cnt = 0;
	uint8_t check_cnt = 3;	//多检测几次防止误检
	
	do
	{
		int16_t adc_value = 0;
		
		_delay_ms(50);
		
		if ((adc_value = adc_read_value(V_BAT_ADC_CHANNEL)) != -1)
		{
			g_cur_bat_voltage = ((float)adc_value / 1024 * DEFAULT_INTERNAL_REFERENCE_VOL * 11);
		}
		
		if ((adc_value = adc_read_value(V_IN_ADC_CHANNEL)) != -1)
		{
			g_cur_charge_voltage = ((float)adc_value / 1024 * DEFAULT_INTERNAL_REFERENCE_VOL * 11);
		}
		
		if ((g_cur_bat_voltage < bat_managent.bat_low_vol) && (g_cur_charge_voltage < MINIMUM_CHARGING_VOLTAGE))
		{
			check_debounce_cnt++;
		}
		else
		{
			check_debounce_cnt--;
		}
	}
	while (--check_cnt != 0);

	if (check_debounce_cnt > 0)
	{
		/* 检测到PIN32脚为高电平并且不在充电,PIN2就输出低电平,关机 */
		PIN2_OUTPUT_LOW();
	}
}

void battery_read_voltage(void)
{
	int16_t adc_value = 0;
	static uint8_t check_debounce_cnt = 0;
	
	if ((adc_value = adc_read_value(V_BAT_ADC_CHANNEL)) != -1)
	{
		g_cur_bat_voltage = ((float)adc_value / 1024 * DEFAULT_INTERNAL_REFERENCE_VOL * 11);
	}
	
	if ((adc_value = adc_read_value(V_IN_ADC_CHANNEL)) != -1)
	{
		g_cur_charge_voltage = ((float)adc_value / 1024 * DEFAULT_INTERNAL_REFERENCE_VOL * 11);
	}
	
	if ((g_cur_bat_voltage < bat_managent.bat_low_vol) && (g_cur_charge_voltage < MINIMUM_CHARGING_VOLTAGE))
	{
		if (check_debounce_cnt++ > MAX_CHECK_LOW_POWER_DEBOUNCE_CNT)	//检测低电量消抖次数，可以看情况设置
		{
			/* 检测到PIN32脚为高电平并且不在充电,PIN2就输出低电平,关机 */
			PIN2_OUTPUT_LOW();
			check_debounce_cnt = 0;
		}
	}
	else
	{
		check_debounce_cnt = 0;
	}
}

void battery_voltage_sample_process(void)
{
	static uint32_t pre_time = 0;
	
	if (millis() - pre_time >= BATTERY_SAMPLE_INTERVAL)
	{
		pre_time = millis();
		
		battery_read_voltage();
	}
}

uint16_t battery_get_cur_bat_voltage(void)
{
	return g_cur_bat_voltage;
}

uint16_t battery_get_cur_charge_voltage(void)
{
	return g_cur_charge_voltage;
}

uint16_t battery_get_full_voltage(void)
{
	return bat_managent.bat_full_vol;
}

uint16_t battery_get_low_voltage(void)
{
	return bat_managent.bat_low_vol;
}

uint8_t battery_get_percentage(void)
{
	uint8_t percentage = 0;
	
	if (g_cur_bat_voltage >= bat_managent.bat_full_vol)
	{
		percentage = 100;
	}
	else
	if (g_cur_bat_voltage > bat_managent.bat_low_vol)
	{
		percentage = (float)(g_cur_bat_voltage - bat_managent.bat_low_vol) / (bat_managent.bat_full_vol - bat_managent.bat_low_vol) * 100;
	}
	
	return percentage;
}

int8_t battery_set_bat_managent(Bat_managent_t *bat_m)
{
	if (bat_m->bat_full_vol <= bat_m->bat_low_vol)
	{
		return -1;
	}
	
	memcpy(&bat_managent, bat_m, sizeof(Bat_managent_t));
	
	eeprom_write_bytes(EEPROM_START_ADDR, (uint8_t*)&bat_managent, sizeof(bat_managent));
	
	return 0;
}



