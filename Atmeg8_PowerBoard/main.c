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
	adc_init();
	power_pin_init();
	
	sei();
	uint32_t pre_time = 0;

    while (1) 
    {
		cmd_receive();
		device_state_manage();
		
		if (millis() - pre_time >= 2000)
		{
			pre_time = millis();
			uart_send_byte('A');
		}
    }
}

