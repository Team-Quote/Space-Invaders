// SW_features.c
// Course number:
// Term: 
// Project number: 
// Feature description: A brief summary on feature responsibility and team member assignment
// Team #:
// Team members:

// Header files needed for this program
// Include C library header files, microntroller header files, and 
// project module header files. 


/*
 * Feature initialization
 */

// Switch 1 (PF4): Toggle fire
// Switch 2 (PF0): Toggle start / reset

#include "SW_features.h"
#include "..\tm4c123gh6pm.h"

volatile bool sw1_event = false; // Switch 1 (PF4)
volatile bool sw2_event = false; // Switch 2 (PF0)

static void DebounceDelay(void){
    for(volatile uint32_t t=0; t<800000; t++) {}
}

// Initialize Switches
void SW_Init(void){
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;    // Enable Port F clock
    while((SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R5)==0) {}

    // Unlock PF0
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= SW2_MASK;

    GPIO_PORTF_DIR_R &= ~SW_MASK;
    GPIO_PORTF_AFSEL_R &= ~SW_MASK;
    GPIO_PORTF_DEN_R |= SW_MASK;
    GPIO_PORTF_PCTL_R &= ~PCTL_PIN;
    GPIO_PORTF_AMSEL_R &= ~SW_MASK;
    GPIO_PORTF_PUR_R |= SW_MASK;

    // PF4 falling-edge interrupt
    GPIO_PORTF_IS_R &= ~SW_MASK;   // edge-sensitive
    GPIO_PORTF_IBE_R &= ~SW_MASK;  // not both edges
    GPIO_PORTF_IEV_R &= ~SW_MASK;  // falling edge
    GPIO_PORTF_ICR_R = SW_MASK;    // clear any prior flag
    GPIO_PORTF_IM_R |= SW_MASK;    // arm interrupt

    // NVIC: GPIO Port F is interrupt #30
    NVIC_PRI7_R = (NVIC_PRI7_R & ~PORTF_PRI_BITS) | (PORTF_INT_PRI << 21);
    NVIC_EN0_R |= 0x40000000;

}

// Returns true once per press (clears event)
bool SW1_WasPressed(void){
    if(sw1_event){
        sw1_event = false;
        return true;
    }
    return false;
}


// Returns true once per press (clear event)
bool SW2_WasPressed(void){
    if(sw2_event){
        sw2_event = false;
        return true;
    }
    return false;
}


// This ISR must exist exactly once in your whole project
void GPIOPortF_Handler(void){
    DebounceDelay();

    // PF4
    if(GPIO_PORTF_RIS_R & SW1_MASK) {
        GPIO_PORTF_ICR_R |= SW1_MASK;    // acknowledge
        sw1_event = true;
    }

    // PF0
    if(GPIO_PORTF_RIS_R & SW2_MASK){
        GPIO_PORTF_ICR_R = SW2_MASK;
        sw2_event = true;
    }
}