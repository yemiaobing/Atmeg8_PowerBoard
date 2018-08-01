#include "timer.h"

static volatile uint32_t timer_count = 0;

ISR(TIMER1_OVF_vect)
{
	TCNT1H = (0xFFFF-125) >> 8;
	TCNT1L = (0xFFFF-125) & 0xFF;
	
	timer_count++;
}
/************************************************************************/
/* ��Ӳ����ʱ��1����1ms�ж����������ʱ��                                 */
/************************************************************************/
void timer1_init(void)
{
	TCCR1B |= (1 << CS11); //��ʱ��ʱ����ϵͳʱ��8��Ƶ�õ� 1000000/8=125000
	TIMSK |= 1 << TOIE1; //��������ж�
	TCNT1H = (0xFFFF-125) >> 8;
	TCNT1L = (0xFFFF-125) & 0xFF;
}

uint32_t millis(void)
{
	return timer_count;
}
