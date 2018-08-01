#include "timer.h"

static volatile uint32_t timer_count = 0;

ISR(TIMER1_OVF_vect)
{
	TCNT1H = (0xFFFF-125) >> 8;
	TCNT1L = (0xFFFF-125) & 0xFF;
	
	timer_count++;
}
/************************************************************************/
/* 用硬件定时器1当作1ms中断来做软件定时器                                 */
/************************************************************************/
void timer1_init(void)
{
	TCCR1B |= (1 << CS11); //定时器时钟由系统时钟8分频得到 1000000/8=125000
	TIMSK |= 1 << TOIE1; //开启溢出中断
	TCNT1H = (0xFFFF-125) >> 8;
	TCNT1L = (0xFFFF-125) & 0xFF;
}

uint32_t millis(void)
{
	return timer_count;
}
