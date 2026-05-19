// Nokia5110.c
// Course number:
// Term: 
// Project number: 
// Driver description: A brief summary on driver responsibility and team member assignment
// Team #:
// Team members:


// Blue Nokia 5110: interface with SSI2
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PC5
// SSI0Fss       (CE,  pin 2) connected to PB5
// Data/Command  (DC,  pin 3) connected to PC4
// SSI0Tx        (Din, pin 4) connected to PB7
// SSI0Clk       (Clk, pin 5) connected to PB4
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Header files needed for this program
// Include C library header files, microntroller header files, and 
// project module header files. 
#include "Nokia5110.h"
#include "..\tm4c123gh6pm.h"
#include "stdint.h"

#define DC                  (*((volatile uint32_t *)0x40006040)) // PC4 masked address     
#define DC_COMMAND          0
#define DC_DATA             0x10

#define RESET               (*((volatile uint32_t *)0x40006080)) // PC5 masked address
#define RESET_LOW           0
#define RESET_HIGH          0x20



enum typeOfWrite{
  COMMAND,
  DATA
};

static void lcdwrite(enum typeOfWrite type, uint8_t message){
  if(type == COMMAND){
    while((SSI2_SR_R & SSI_SR_BSY) == SSI_SR_BSY){}
    DC = DC_COMMAND;
    SSI2_DR_R = message;
    while((SSI2_SR_R & SSI_SR_BSY) == SSI_SR_BSY){}
  }else{
    while((SSI2_SR_R & SSI_SR_TNF) == 0){}
    DC = DC_DATA;
    SSI2_DR_R = message;
  }
}



/*
 * Module initialization
 */

//********Nokia5110_Init*****************
// Initialize Nokia 5110 48x84 LCD by sending the proper
// commands to the PCD8544 driver.  One new feature of the
// LM4F120 is that its SSIs can get their baud clock from
// either the system clock or from the 16 MHz precision
// internal oscillator.  If the system clock is faster than
// 50 MHz, the SSI baud clock will be faster than the 4 MHz
// maximum of the Nokia 5110.
// inputs: none
// outputs: none
// assumes: system clock rate of 50 MHz or less
void Nokia5110_Init(void) {
    volatile uint32_t delay;

    // clocks: SSI2 + Port B + Port C
    SYSCTL_RCGCSSI_R |= 0x04; // activate SSI2
    SYSCTL_RCGCGPIO_R |= 0x06; // activate Port B and Port C
    delay = SYSCTL_RCGCGPIO_R; // allow time to finish activating

    // PB4 = SSI2CLK, PB5 = SSI2FSS, PB7 = SSI2TX
    GPIO_PORTB_DIR_R |= 0xB0; // make PB4, PB5, PB7 output
    GPIO_PORTB_AFSEL_R |= 0xB0; // enable alt funct on
    GPIO_PORTB_DEN_R |= 0xB0; // enable digital I/O on PB4, PB5, PB7
    GPIO_PORTB_PCTL_R   = (GPIO_PORTB_PCTL_R & 0x0F00FFFF) | 0x20220000; // configure PB4, PB5, PB7 as SSI2
    GPIO_PORTB_AMSEL_R &= ~0xB0; // disable analog functionality on PB4, PB5, PB7

    // PC4 = DC, PC5 = RESET
    GPIO_PORTC_DIR_R |= 0x30; // make PC4, PC5 output
    GPIO_PORTC_AFSEL_R &= ~0x30; // disable alt funct on PC4, PC5
    GPIO_PORTC_DEN_R |= 0x30; // enable digital I/O on PC4, PC5
    GPIO_PORTC_PCTL_R &= ~0x00FF0000; // configure PC4, PC5 as GPIO
    GPIO_PORTC_AMSEL_R &= ~0x30; // disable analog functionality on PC4, PC5

    // SSI2 configuration
    SSI2_CR1_R &= ~SSI_CR1_SSE; // disable SSI
    SSI2_CR1_R &= ~SSI_CR1_MS; // master mode
    SSI2_CC_R   = (SSI2_CC_R & ~SSI_CC_CS_M) | SSI_CC_CS_SYSPLL; // clock source
    SSI2_CPSR_R = 20; // safe for both 50 MHz and 80 MHz
    SSI2_CR0_R &= ~(SSI_CR0_SCR_M | SSI_CR0_SPH | SSI_CR0_SPO); // SCR = 0, SPH = 0, SPO = 0
    SSI2_CR0_R  = (SSI2_CR0_R & ~SSI_CR0_FRF_M) | SSI_CR0_FRF_MOTO; // Freescale format
    SSI2_CR0_R  = (SSI2_CR0_R & ~SSI_CR0_DSS_M) | SSI_CR0_DSS_8; // 8-bit data
    SSI2_CR1_R |= SSI_CR1_SSE;                  // enable SSI2
    
    // reset the LCD to a known state
    RESET = RESET_LOW;                    // reset the LCD to a known state
    for(delay=0; delay<10; delay=delay+1);// delay minimum 100 ns
    RESET = RESET_HIGH;                   // negative logic

    lcdwrite(COMMAND, 0x21);              // chip active; horizontal addressing mode (V = 0); use extended instruction set (H = 1)
                                            // set LCD Vop (contrast), which may require some tweaking:
    lcdwrite(COMMAND, CONTRAST);          // try 0xB1 (for 3.3V red SparkFun), 0xB8 (for 3.3V blue SparkFun), 0xBF if your display is too dark, or 0x80 to 0xFF if experimenting
    lcdwrite(COMMAND, 0x04);              // set temp coefficient
    lcdwrite(COMMAND, 0x14);              // LCD bias mode 1:48: try 0x13 or 0x14

    lcdwrite(COMMAND, 0x20);              // we must send 0x20 before modifying the display control mode
    lcdwrite(COMMAND, 0x0C);              // set display control to normal mode: 0x0D for inverse

}
  //********Nokia5110_OutChar*****************
// Print a character to the Nokia 5110 48x84 LCD.  The
// character will be printed at the current cursor position,
// the cursor will automatically be updated, and it will
// wrap to the next row or back to the top if necessary.
// One blank column of pixels will be printed on either side
// of the character for readability.  Since characters are 8
// pixels tall and 5 pixels wide, 12 characters fit per row,
// and there are six rows.
// inputs: data  character to print
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutChar(uint8_t data){
  uint16_t i;
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
  for(i=0; i<5; i=i+1){
    lcdwrite(DATA, ASCII[data - 0x20][i]);
  }
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
}

//********Nokia5110_OutString*****************
// Print a string of characters to the Nokia 5110 48x84 LCD.
// The string will automatically wrap, so padding spaces may
// be needed to make the output look optimal.
// inputs: ptr  pointer to NULL-terminated ASCII string
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutString(uint8_t *ptr){
  while(*ptr){
    Nokia5110_OutChar(*ptr);
    ptr = ptr + 1;
  }
}

//********Nokia5110_OutUDec*****************
// Output a 16-bit number in unsigned decimal format with a
// fixed size of five right-justified digits of output.
// Inputs: n  16-bit unsigned number
// Outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutUDec(uint16_t n){
  if(n < 10){
    Nokia5110_OutString((uint8_t *)"    ");
    Nokia5110_OutChar(n+'0'); /* n is between 0 and 9 */
  } else if(n<100){
    Nokia5110_OutString((uint8_t *)"   ");
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  } else if(n<1000){
    Nokia5110_OutString((uint8_t *)"  ");
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else if(n<10000){
    Nokia5110_OutChar(' ');
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else {
    Nokia5110_OutChar(n/10000+'0'); /* ten-thousands digit */
    n = n%10000;
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
}

//********Nokia5110_SetCursor*****************
// Move the cursor to the desired X- and Y-position.  The
// next character will be printed here.  X=0 is the leftmost
// column.  Y=0 is the top row.
// inputs: newX  new X-position of the cursor (0<=newX<=11)
//         newY  new Y-position of the cursor (0<=newY<=5)
// outputs: none
void Nokia5110_SetCursor(uint8_t newX, uint8_t newY){
  if((newX > 11) || (newY > 5)){        // bad input
    return;                             // do nothing
  }
  // multiply newX by 7 because each character is 7 columns wide
  lcdwrite(COMMAND, 0x80|(newX*7));     // setting bit 7 updates X-position
  lcdwrite(COMMAND, 0x40|newY);         // setting bit 6 updates Y-position: 3 bits for positions: 0 - 5
}

//********Nokia5110_Clear*****************
// Clear the LCD by writing zeros to the entire screen and
// reset the cursor to (0,0) (top left corner of screen).
// inputs: none
// outputs: none
void Nokia5110_Clear(void){
  uint16_t i;
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, 0x00);
  }
  Nokia5110_SetCursor(0, 0);
}

//********Nokia5110_DrawFullImage*****************
// Fill the whole screen by drawing a 48x84 bitmap image.
// inputs: ptr  pointer to 504 byte bitmap
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DrawFullImage(const uint8_t *ptr){
  uint16_t i;
  Nokia5110_SetCursor(0, 0);
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, ptr[i]);
  }
}

uint8_t Screen[SCREENW * SCREENH / 8];

//********Nokia5110_PrintBMP*****************
// inputs: xpos      horizontal position of bottom left corner of image
//         ypos      vertical position of bottom left corner of image
//         ptr       pointer to a 16 color BMP image
//         threshold grayscale colors above this number make corresponding pixel 'on'
void Nokia5110_PrintBMP(uint8_t xpos, uint8_t ypos, const uint8_t *ptr, uint8_t threshold){
  int32_t width = ptr[18], height = ptr[22], i, j;
  uint16_t screenx, screeny;
  uint8_t mask;

  if((height <= 0) ||              // bitmap encoded unexpectedly
     ((width % 2) != 0) ||         // width must be even
     ((xpos + width) > SCREENW) || // right side clipped
     (ypos < (height - 1)) ||      // top clipped
     (ypos > SCREENH)){            // bottom clipped
    return;
  }

  if(threshold > 14){
    threshold = 14;
  }

  screeny = ypos / 8;
  screenx = xpos + SCREENW * screeny;
  mask = ypos % 8;
  mask = 0x01 << mask;
  j = ptr[10];   // image data offset

  for(i = 1; i <= (width * height / 2); i++){
    // left pixel = upper 4 bits
    if(((ptr[j] >> 4) & 0x0F) > threshold){
      Screen[screenx] |= mask;
    } else {
      Screen[screenx] &= ~mask;
    }
    screenx++;

    // right pixel = lower 4 bits
    if((ptr[j] & 0x0F) > threshold){
      Screen[screenx] |= mask;
    } else {
      Screen[screenx] &= ~mask;
    }
    screenx++;
    j++;

    if((i % (width / 2)) == 0){ // end of row
      if(mask > 0x01){
        mask = mask >> 1;
      } else {
        mask = 0x80;
        screeny = screeny - 1;
      }

      screenx = xpos + SCREENW * screeny;

      // skip BMP row padding
      switch((width / 2) % 4){
        case 0: j += 0; break;
        case 1: j += 3; break;
        case 2: j += 2; break;
        case 3: j += 1; break;
      }
    }
  }
}

void Nokia5110_ClearBuffer(void){
  uint16_t i;
  for(i = 0; i < (SCREENW * SCREENH / 8); i++){
    Screen[i] = 0;
  }
}

void Nokia5110_ClearShip(void){
  uint16_t i;
  for(i = (SCREENW * (SCREENH - 8) / 8); i < (SCREENW * SCREENH / 8); i++){
    Screen[i] = 0;
  }
}

void Nokia5110_ClearInvaders(void){
  uint16_t i;
  for(i = 0; i < (SCREENW * SCREENH / 16); i++){
    Screen[i] = 0;
  }
}

void Nokia5110_SetPixel(uint8_t x, uint8_t y){
  uint16_t pixelByte;
  uint8_t pixelBit;

  if((x < SCREENW) && (y < SCREENH)){
    pixelByte = ((y / 8) * SCREENW) + x;
    pixelBit = y % 8;
    Screen[pixelByte] |= (1U << pixelBit);
  }
}

void Nokia5110_ClearPixel(uint8_t x, uint8_t y){
  uint16_t pixelByte;
  uint8_t pixelBit;

  if((x < SCREENW) && (y < SCREENH)){
    pixelByte = ((y / 8) * SCREENW) + x;
    pixelBit = y % 8;
    Screen[pixelByte] &= ~(1U << pixelBit);
  }
}

void Nokia5110_DisplayBuffer(void){
  Nokia5110_DrawFullImage(Screen);
}


