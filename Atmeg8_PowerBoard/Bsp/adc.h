#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

extern void adc_init(void);
extern int16_t adc_read_value(unsigned char adc_channel);

#endif /* ADC_H_ */