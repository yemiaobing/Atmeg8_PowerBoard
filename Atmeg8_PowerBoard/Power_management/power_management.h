#ifndef POWER_MANAGEMENT_H_
#define POWER_MANAGEMENT_H_

#include <avr/io.h>
#include <stdint.h>
#include "../Bsp/eeprom.h"
#include "../Bsp/adc.h"

#define PIN1_INPUT_VALUE ((PIND & (1 << PIND3)) > 0)	/* 检测开关机按钮 */

#define PIN32_INPUT_VALUE ((PIND & (1 << PIND2)) > 0)   /* 检测主板电源 */

#define PIN2_OUTPUT_HIGH() {PORTD |= 1 << PD4;}
#define PIN2_OUTPUT_LOW() {PORTD &= ~(1 << PD4);}	/* 断掉单片机电源 */

#define PIN28_OUTPUT_HIGH() {PORTC |= 1 << PC5;}    /* 输出脉冲 */
#define PIN28_OUTPUT_LOW() {PORTC &= ~(1 << PC5);}   

#define V_BAT_ADC_CHANNEL	ADC7_CHANNEL
#define V_IN_ADC_CHANNEL	ADC6_CHANNEL

#define DEFAULT_BATTERY_FULL_VOL	12600	//unit:mv
#define DEFAULT_BATTERY_LOW_VOL	9000	//unit:mv

#define DEFAULT_INTERNAL_REFERENCE_VOL	2500 //unit:mv

#define BATTERY_SAMPLE_INTERVAL 2000  //unit: ms

#define MINIMUM_CHARGING_VOLTAGE 11000

#define BAT_MANAGENT_EEPROM_ADDR EEPROM_START_ADDR

#define MAX_CHECK_LOW_POWER_DEBOUNCE_CNT	3

typedef struct
{
	uint16_t bat_full_vol;
	uint16_t bat_low_vol;
	uint16_t internal_refer_vol;
	uint8_t internal_vol_div_resistor1;
	uint8_t internal_vol_div_resistor2;
}Bat_managent_t;

extern void power_pin_init(void);
extern void power_on(void);
extern void key_scan(void);
extern void battery_init(void);
extern void battery_voltage_sample_process(void);
extern void battery_read_voltage(void);
extern uint16_t battery_get_cur_bat_voltage(void);
extern uint16_t battery_get_cur_charge_voltage(void);
extern uint16_t battery_get_full_voltage(void);
extern uint16_t battery_get_low_voltage(void);
extern uint8_t battery_get_percentage(void);
extern int8_t battery_set_bat_managent(Bat_managent_t *bat_m);
extern void need_enter_power_off_status(void);
#endif /* POWER_MANAGEMENT_H_ */