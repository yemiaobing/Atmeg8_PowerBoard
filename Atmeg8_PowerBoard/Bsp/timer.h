#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>

extern void timer1_init(void);
extern uint32_t millis(void);
#endif /* TIMER_H_ */