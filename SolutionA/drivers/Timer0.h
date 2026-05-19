#ifndef __TIMER0INTS_H__ // do not include more than once
#define __TIMER0INTS_H__
#include <stdint.h>

// ***************** Timer0_Init ****************
// Activate Timer0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer0_Init(void(*task)(void), uint32_t period);

#endif // __TIMER0INTS_H__