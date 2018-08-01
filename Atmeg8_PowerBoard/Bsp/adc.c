#include "adc.h"

void adc_init(void)
{
	ADMUX |= 1 << 6;
	ADCSRA |= 3; //8分频 adc时钟等于1000000/8 =125000hz
	ADCSRA |= 1 << 7; //adc enable
}

int16_t adc_read_value(unsigned char adc_channel)
{
	int16_t adc_value = 0;
	
	if ((adc_channel >= 8 && adc_channel <= 13) || (adc_channel > 0xF))
	{
		return -1;
	}
	
	ADMUX &= ~0x0F;
	ADMUX |= adc_channel;
	ADCSRA |= 1 << 4; //clear interrupt flag
	ADCSRA |= 1 << 6; //start conversion
	while(!(ADCSRA & (1 << 4)));
	
	adc_value = ADCL;
	adc_value |= ADCH << 8;
	
	return adc_value;
}