#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

#define ADC0_CHANNEL	0
#define ADC1_CHANNEL	1
#define ADC2_CHANNEL	2
#define ADC3_CHANNEL	3
#define ADC4_CHANNEL	4
#define ADC5_CHANNEL	5
#define ADC6_CHANNEL	6
#define ADC7_CHANNEL	7
#define ADC14_CHANNEL	14
#define ADC15_CHANNEL	15

extern void adc_init(void);
extern int16_t adc_read_value(unsigned char adc_channel);

#endif /* ADC_H_ */