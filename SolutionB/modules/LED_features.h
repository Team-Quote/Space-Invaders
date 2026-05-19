#ifndef LED_H
#define LED_H
// LED_features.h
// Course number:
// Term: 
// Project number: 
// Feature description: A brief summary on feature responsibility and team member assignment
// Team #:
// Team members:

#include <stdbool.h>
#include <stdint.h>

#define LED     (*(( volatile unsigned long *)0x40025038))  // Verify Correct Offset

// LED bit masks on Port F
#define LED_RED     (0x02)      // PF1 (0000 0010)
#define LED_BLUE    (0x04)      // PF2 (0000 0100)
#define LED_GREEN   (0x08)      // PF3 (0000 1000)

#define LED_PINS    (0x0E)  // RED/BLUE/GREEN -> PF1/PF2/PF3

void LED_Init(void);

void RED_LED(void);
void GREEN_LED(void);
void BLUE_LED(void);


#endif