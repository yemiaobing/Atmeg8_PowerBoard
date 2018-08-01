#ifndef BSP_H_
#define BSP_H_

#define F_CPU 1000000UL
#include "util/delay.h"

#define DISABLE_INT() cli();
#define ENABLE_INT()  sei();

#endif /* BSP_H_ */