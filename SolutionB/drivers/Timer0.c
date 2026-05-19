// Timer0.c 
// Course number:
// Term: 
// Project number: 
// Driver description: A brief summary on driver responsibility and team member assignment
// Team #:
// Team members:

// Header files needed for this program
// Include C library header files, microntroller header files, and 
// project module header files. 
#include "Timer0.h"
#include "..\tm4c123gh6pm.h"
#include "stdint.h"

void (*PeriodicTask0)(void);   // user function

void Timer0_Init(void(*task)(void), uint32_t period) {
    
    SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  PeriodicTask0 = task;          // user function
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R |= (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R |= 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R |= 0x00000001;    // 10) enable TIMER0A
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER0A timeout
  (*PeriodicTask0)();                // execute user task
}

