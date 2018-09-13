#include <avr/io.h>
#include "Bsp/bsp.h"
#include "Bsp/bsp_uart.h"
#include "Bsp/timer.h"
#include <avr/interrupt.h>
#include "Bsp/adc.h"
#include "Cmd//cmd.h"
#include "Power_management/power_management.h"

int main(void)
{
	uart_init(4800);
	timer1_init();
	battery_init();
	power_pin_init();
	sei();
	
	power_on();
	
	need_enter_power_off_status();
	
	_delay_ms(5000);	//延时等待主板上电
    while (1) 
    {
		cmd_receive();
		key_scan();
		battery_voltage_sample_process();
    }
}

