// DAC.h
// Course number:
// Term: 
// Project number: 
// Driver description: A brief summary on driver responsibility and team member assignment
// Team #:
// Team members:

#ifndef DAC_H
#define DAC_H
#include <stdint.h>

// Header files contain the prototypes for public functions
// this file explains what the module does

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void);


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint8_t data);

#endif // DAC_H