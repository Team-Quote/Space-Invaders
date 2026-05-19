#ifndef __TIMER1INTS_H__ // do not include more than once
#define __TIMER1INTS_H__
// Timer1A.h
// Course number:
// Term: 
// Project number: 
// Driver description: A brief summary on driver responsibility and team member assignment
// Team #:
// Team members:

#include <stdint.h>
// ***************** Timer1_Init ****************
// Activate Timer1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1_Init(void(*task)(void), uint32_t period);

#endif // __TIMER1INTS_H__