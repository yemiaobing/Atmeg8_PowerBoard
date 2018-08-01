#ifndef POWER_MANAGEMENT_H_
#define POWER_MANAGEMENT_H_

#include <avr/io.h>

#define PIN1_INPUT_VALUE ((PIND & (1 << PIND3)) > 0)	/* ��⿪�ػ���ť */

#define PIN32_INPUT_VALUE ((PIND & (1 << PIND2)) > 0)   /* ��������Դ */

#define PIN2_OUTPUT_HIGH() {PORTD |= 1 << PD4;}
#define PIN2_OUTPUT_LOW() {PORTD &= ~(1 << PD4);}	/* �ϵ���Ƭ����Դ */

#define PIN28_OUTPUT_HIGH() {PORTC |= 1 << PC5;}    /* ������� */
#define PIN28_OUTPUT_LOW() {PORTC &= ~(1 << PC5);}   
	
#define LONG_KEY_TIME	6000 //6s ��������ʱ��

#define SHAKING_TIME    20  //unit:ms ���ڰ�������

typedef enum
{
	POWER_ON_DETECT = 0,
	POWER_ON_WAIT_2_SECONDS,
	POWER_ON_OUTPUT_20MS_PULSE,
	NORMAL_WORK,
	POWER_OFF_DETECT,
	POWER_OFF_OUTPUT_20MS_PULSE,
	POWER_OFF_WAIT_MAIN_BOARD_OFF,
	FORCE_SHUTDOWN_DETECT,
	FORCE_SHUTDOWN_OUTPUT_6S_PULSE
}DEVICE_STATE_E;

typedef enum
{
	DETECT_NO_KEY_PRESSED,
	DETECT_KEY_PRESSED,
	DETECT_KEY_LONG_PRESSED
}KEY_STATE_T;

extern void power_pin_init(void);
extern void device_state_manage(void);

#endif /* POWER_MANAGEMENT_H_ */