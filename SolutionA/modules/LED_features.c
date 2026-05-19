// LED_features.c
// Course number:
// Term: 
// Project number: 
// Feature description: A brief summary on feature responsibility and team member assignment
// Team #:
// Team members:

// Header files needed for this program
// Include C library header files, microntroller header files, and 
// project module header files. 
#include "LED_features.h"
#include "..\tm4c123gh6pm.h"

/*
 * Feature initialization
 */

void LED_Init(void){
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;    // Enable Port F clock
    while((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R5)==0) {}

    GPIO_PORTF_DIR_R |= LED_PINS;   // Set PF1, PF2, PF3 as output
    GPIO_PORTF_AFSEL_R &= ~LED_PINS;  // Disable alternate function
    GPIO_PORTF_DEN_R |= LED_PINS;   // Enable digital function
    GPIO_PORTF_PCTL_R &= ~0x0000FFF0;  // Clear PCTL for PF1, PF2, PF3
    GPIO_PORTF_AMSEL_R &= ~LED_PINS;  // Disable analog function
    GPIO_PORTF_DATA_R &= ~LED_PINS;  // Turn off all LEDs initially
}

void RED_LED(void){
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LED_PINS) | LED_RED;
}

void GREEN_LED(void){
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LED_PINS) | LED_GREEN;
}

void BLUE_LED(void){
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LED_PINS) | LED_BLUE;
}