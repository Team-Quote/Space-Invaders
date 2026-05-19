#ifndef SW_FEATURES_H
#define SW_FEATURES_H
// SW_features.h
// Course number:
// Term: 
// Project number: 
// Feature description: A brief summary on feature responsibility and team member assignment
// Team #:
// Team members:

#include <stdbool.h>
#include <stdint.h>

// Switch 1 (PF4)
#define SW1_MASK    0x10        // 0001 0000

// Switch 2 (PF0)
#define SW2_MASK    0x01      // 0000 0001
#define SW_MASK     (SW1_MASK | SW2_MASK)    // 0001 0001
#define PCTL_PIN    0x000F000F
#define PORTF_PRI_BITS  0x00E00000
#define PORTF_INT_PRI   5U          // priority 0-7 (lower = higher priority)

extern volatile bool sw1_event;
extern volatile bool sw2_event;

void SW_Init(void);
bool SW1_WasPressed(void);
bool SW2_WasPressed(void);

#endif
