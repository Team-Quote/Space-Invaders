// ADC0SS1.c
// Course number:
// Term: 
// Project number: 
// Driver description: A brief summary on driver responsibility and team member assignment
// Team #:
// Team members:

// Header files needed for this program
// Include C library header files, microntroller header files, and 
// project module header files. 

#include "..\tm4c123gh6pm.h"
#include <stdint.h>
#include "ADC0SS1.h"

#define ADC0_PSSI_SS1  0x0002
#define ADC0_ISC_SS1   0x0002
#define ADC0_RIS_SS1   0x0002

/*
 * 	Module initialization
		Requirement is 
		PE3 / AIN0
		ADC0 SS1
		
 */

void ADC0SS1_Init_AIN0_PE3(void) {
	// TODO: student implementation
  // ADC0 Sample Sequencer 1
  // PE3 / AIN0
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;  // 1) activate clock for Port E
	while ((SYSCTL_RCGCGPIO_R&SYSCTL_RCGCGPIO_R4)!=SYSCTL_RCGCGPIO_R4){}
		
	GPIO_PORTE_DIR_R		&= ~0x08; 	// 2) make PE3 input
	GPIO_PORTE_AFSEL_R	|= 0x08; 		// 3) enable alternate function on PE3
	GPIO_PORTE_DEN_R		&= ~0x08; 	// 4) disable digital I/O on PE3
	GPIO_PORTE_AMSEL_R	|= 0x08;		// 5) enable analog function on PE3
	SYSCTL_RCGC0_R |= 0x00010000;   // 6) activate ADC0 
	while ((SYSCTL_RCGC0_R&0x00010000)!=0x00010000){}
	
	SYSCTL_RCGC0_R &= ~0x00000300;  // 7) configure for 125K  
	ADC0_SSPRI_R		=  0x3012;			// 8) set sequencer priority
																	// se1 highest priority then seq0, seq2, seq3
	ADC0_ACTSS_R	&= ~0x0002;				// 9) disable sample sequencer 1 during configuration
	ADC0_EMUX_R		&= ~0x00F0;				// 10) seq1 is software trigger
	ADC0_SSMUX1_R &= ~0x000F;				// 11) set first sample in SS1 to Ain0 (PE3)
																	// sample 0 = channel 0
	ADC0_SSCTL1_R	= 0x0006;					// 12) set sample control bits for sample 0
																	// D0 = 0 (single-ended)
																	// END0 = 1 (this is the only sample in the sequence)
																	// IE0 = 1 (flag set at completion)
																	// TS0 = 0 (use external analog input, not temp sensor)
	ADC0_ACTSS_R	|= 0x0002;				// 13) enable sample sequencer 1
}

uint32_t ADC0SS1_In(void) {
	// TODO: trigger conversion, wait, read result
  uint32_t result;

  // 1) initiate SS1
  ADC0_PSSI_R = ADC0_PSSI_SS1;

  // 2) wait for conversion complete
  while((ADC0_RIS_R & ADC0_RIS_SS1) == 0){}

  // 3) read 12-bit result
  result = ADC0_SSFIFO1_R & 0x0FFF;

  // 4) acknowledge completion
  ADC0_ISC_R = ADC0_ISC_SS1;

  return result;
}