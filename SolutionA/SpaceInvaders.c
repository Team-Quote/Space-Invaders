// SpaceInvaders.c
// Course number: 
// Term: 
// Project number: 
// Module description:
// A brief summary on modules tested in this module test file,
// including module list, responsibility and team member assignment
// Team #: 
// Team members: 


// Starter file for CECS 347 Project 3 - Space Invaders
// Runs on TM4C123
// Created by Dr. Min He
// April 7, 2026

// Reference:
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* The original example code comes the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
 // ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE3 (Ain0)
// Slide pot pin 3 connected to pne side of the 1k resistor
// Onboard sw1(left push button): fire button 
// Onboard sw2(right push button): game start button

// Nokia 5110: interface with SSI2
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST) connected to PC5
// SSI0Fss       (CE) connected to PB5
// Data/Command  (DC) connected to PC4
// SSI0Tx        (Din) connected to PB7
// SSI0Clk       (Clk) connected to PB4
// 3.3V          (Vcc) power
// back light    (BL) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd) ground

// **************DAC_Init*********************
// Initialize 3-bit DAC, called once 
// DAC bit 0 on PB0 (least significant bit)
// DAC bit 1 on PB1
// DAC bit 2 on PB2
// DAC bit 3 on PB3 (most significant bit)
// Input: none
// Output: none

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>

// Drivers
#include "ADC0SS1.h"
#include "PLL.h"
#include "Nokia5110.h"
#include "Timer1A.h"
#include "Timer0.h"
#include "DAC.h"
#include "Sound.h"
// Modules
#include "SW_features.h"
#include "LED_features.h"

enum game_status{OVER,ON};
enum life_status{DEAD, ALIVE};
enum enemy_posture{CLOSE, OPEN};

#define MAX_X_AXIS 83  // size of the LCD screen 48x84, X axis range: 0 to 83
#define PLAYERW     ((unsigned char)PlayerShip0[18])
#define PLAYERH     ((unsigned char)PlayerShip0[22])
#define ENEMY10W    16  
#define ENEMY10H    10
#define LASERH      9
#define LASERW      2
#define BULLETH     LASERH
#define BULLETW     LASERW
#define SCREENW      84
#define SCREENH      48
#define PLAYER_Y     47
#define PLAYER_MAX_X (SCREENW - PLAYERW)
#define BULLET_START_X(shipx) ((shipx) + ((PLAYERW - BULLETW) / 2))
#define BULLET_START_Y(shipy) ((shipy) - BULLETH) // Just above the ships top edge

struct State {
  uint8_t x;      // x coordinate
  uint8_t y;      // y coordinate
  const uint8_t *image; // ptr->image
  bool life;            // 0=dead, 1=alive
};          
typedef struct State STyp;
STyp Enemy[3];
STyp PlayerShip;
STyp Bullet;
STyp Explosion;

// global variables used for game control
volatile bool time_to_draw = false;
enum game_status game_s = OVER;
uint8_t score = 0;

// ADC Test Global variables and function declaration
volatile uint16_t ADCvalue;
volatile uint8_t x_axis;
volatile uint32_t TickCount10Hz = 0;

uint8_t ADC_To_PlayerX(uint16_t adc){
  uint32_t x;
  x = (adc * PLAYER_MAX_X) / 4095;   // map to 0..66, not 0..83
  if(x > PLAYER_MAX_X){
    x = PLAYER_MAX_X;
  }
  return (uint8_t)x;
}

// Boolean for Rectangle Overlap Detection
static bool RectOverlap(uint8_t ax, uint8_t ay, uint8_t aw, uint8_t ah,
                        uint8_t bx, uint8_t by, uint8_t bw, uint8_t bh){
  int16_t aLeft   = ax;
  int16_t aRight  = ax + aw - 1;
  int16_t aBottom = ay;
  int16_t aTop    = ay - ah + 1;

  int16_t bLeft   = bx;
  int16_t bRight  = bx + bw - 1;
  int16_t bBottom = by;
  int16_t bTop    = by - bh + 1;

  if(aRight < bLeft)  return false;
  if(bRight < aLeft)  return false;
  if(aBottom < bTop)  return false;
  if(bBottom < aTop)  return false;
  return true;
}
												
bool end_after_explosion = false;

// Function prototypes
extern void EnableInterrupts(void);  // defined in startup.s
extern void DisableInterrupts(void); // defined in startup.s

void Game_Init(void);
void Move(void);
void Draw(void);
void Start_Prompt(void);
void End_Prompt(void);
void System_Init(void);
void Delay100ms(unsigned long count);
void GameTick(void);


int main(void){
	System_Init();

  while(1){
    Start_Prompt();
    
		while(game_s==OVER){
      // wait for SW2 to be pressed to start the game
      if(SW2_WasPressed()){
        game_s = ON;
      }
    };
      
		Game_Init(); // all sprites: 3 sprites
    Draw();
    while (game_s==ON) {
      if (time_to_draw){
        Move();
        Draw();
        time_to_draw = false;
      }
    }
    
    End_Prompt();
  }
}

void System_Init(void){
  DisableInterrupts();
  PLL_Init();             // set system clock to 50 MHz
  Sound_Init();
  Timer1_Init(&GameTick, 5000000); // set up Timer1 to generate interrupts at 10 Hz (every 100 ms)
  ADC0SS1_Init_AIN0_PE3();
  Nokia5110_Init();
  SW_Init();
  EnableInterrupts();
}

// Display the game start prompt
void Start_Prompt(void){
  // Press SW2 TO START game prompt
  // Display start prompt on Nokia5110
  Nokia5110_Clear();
  Nokia5110_SetCursor(0, 0);
  Nokia5110_OutString((uint8_t *)"Press SW2 TO START");

}

// Display the game end prompt for 2 seconds
void End_Prompt(void){
  // Layout:
  // GAME OVER
  // Nice try!
  // Your Score!
  //      Score here 
  uint32_t startTick = TickCount10Hz; // record the start time
  Nokia5110_Clear();
  Nokia5110_SetCursor(0,1);
  Nokia5110_OutString((uint8_t *)"GAME OVER");
  Nokia5110_SetCursor(0,3);
  Nokia5110_OutString((uint8_t *)"SCORE:");
  Nokia5110_OutChar((score % 10) + '0'); // display the last digit of the score
  while((TickCount10Hz - startTick) < 30){}
}

// Initialize the game: initialize all sprites and 
// reset refresh control and game status.
void Game_Init(void){
  time_to_draw=false;
  score = 0; // reset score
	game_s = ON;
	end_after_explosion = false;
	
  // Version 1: add enemy initialization with close posture.
  uint8_t i;
  for(i = 0; i < 3; i++) {
    Enemy[i].x = i * 20; // space enemies 20 pixels apart starting at x=10
    Enemy[i].y = 10; // all enemies start at y=10
    Enemy[i].image = SmallEnemyPointA[i]; // start with close posture
    Enemy[i].life = ALIVE; // all enemies start alive
  }
  Bullet.life = DEAD; // no bullet at the beginning

  // Version 2: add player ship initialization
  ADCvalue = ADC0SS1_In();
  PlayerShip.x = ADC_To_PlayerX(ADCvalue);
  PlayerShip.y = PLAYER_Y;
  PlayerShip.image = PlayerShip0;
  PlayerShip.life = ALIVE;

  // Version 3: Add bullet initialization: you can choose Laser or Missile
  Bullet.x = 0;
  Bullet.y = 0;
  Bullet.image = Laser0; // Or Missle0 for preference
  Bullet.life = DEAD; // no bullet at the beginning

  Explosion.x = 0;
  Explosion.y = 0;
  Explosion.image = SmallExplosion0;
  Explosion.life = DEAD; // no explosion at the beginning
}

// Update positions for all alive sprites.
void Move(void){
  uint8_t num_life = 0;
  uint8_t i;
	
	if(end_after_explosion){
  end_after_explosion = false;
  game_s = OVER;
  return;
}
	
	// V3: Move Bullet: detect hit or top of the screen.
	if(Bullet.life == ALIVE){
    if(Bullet.y > 0) {
      Bullet.y -= 1; // 1 pixel per frame upwards
    }

    // Disappears after reaching/passing top
    if (Bullet.y < BULLETH){
      Bullet.life = DEAD;
    } else {
      for(i = 0; i < 3; i++) {
        if(Enemy[i].life == ALIVE && RectOverlap(Bullet.x, Bullet.y, BULLETW, BULLETH, Enemy[i].x, Enemy[i].y, ENEMY10W, ENEMY10H)){
          Enemy[i].life = DEAD; // enemy is dead if hit by bullet
          Bullet.life = DEAD; // bullet disappears after hit
          score++; // increase score for a successful hit

          // V4: If a hit is detected, play the explosion sound and activate the explosion sprite at the position of the enemy that got hit.
          Explosion.x = Enemy[i].x; // set explosion position to enemy position
          Explosion.y = Enemy[i].y;
          Explosion.image = SmallExplosion0;
          Explosion.life = ALIVE; // activate explosion
          Sound_Explosion();
          break; // exit loop after hit is detected
        }
      }
    }
  }
	

  // V1: Move enemies: modify x, check life:alive or dead: dead if right side reaches right screen border or detect a hit
  // Change postures for the enemies: two postures: open/close arms/legs
	// If current position is close, then next position will be open; vise versa.
  for(i = 0; i < 3; i++) {
    if(Enemy[i].life == ALIVE) {
      Enemy[i].x += 1; // V1 = 1 pixel per frame to the right

      if(Enemy[i].x + ENEMY10W >= 84) {
        Enemy[i].life = DEAD; // enemy is dead if it reaches the right border
      } else {
        num_life++;
      }
    }
  }

  // V3: fire only if no bullet is active and at least one enemy still exists
  if((num_life > 0) && (Bullet.life == DEAD) && SW1_WasPressed()){
    Bullet.x = BULLET_START_X(PlayerShip.x);
    Bullet.y = BULLET_START_Y(PlayerShip.y);
    Bullet.image = Laser0;   // or Missile0
    Bullet.life = ALIVE;
    Sound_Shoot();
  }
	// V2: Read ADC and update player ship position: only x coordinate will be changed. 
  ADCvalue = ADC0SS1_In();
  PlayerShip.x = ADC_To_PlayerX(ADCvalue);

  if (num_life==0) {
  if(Explosion.life == ALIVE){
    end_after_explosion = true;
  } else {
    game_s = OVER;
  }
}
  
}

// Update the screen: 
// clear display and update the screen with the 
// current positions of all sprites that are alive.
void Draw(void){
  static uint8_t enemy_posture = CLOSE;  // enemy start with close posture: SmallEnemyPointA
  uint8_t i;
  
  if (game_s==OVER) return;
  
 Nokia5110_ClearBuffer();
  
  // V1:Update live enemies' positions in display buffer: screen[]
  for(i = 0; i < 3; i++) {
    if(Enemy[i].life == ALIVE) {
      if(enemy_posture == CLOSE) {
        Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, SmallEnemyPointA[i], 0); // draw close posture
      } else {
        Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, SmallEnemyPointB[i], 0); // draw open posture
      }
    }
  }
  
  // V2: Update the playership position in display buffer: screen[]
  if(PlayerShip.life == ALIVE) {
    Nokia5110_PrintBMP(PlayerShip.x, PlayerShip.y, PlayerShip.image, 0);
  }  
  // V3: Update the bullet position in display buffer if there is one.
  if (Bullet.life==ALIVE) {
    Nokia5110_PrintBMP(Bullet.x, Bullet.y, Bullet.image, 0);
  }
	// V4: if bullet is just activated, play the shoot sound here
  if(Explosion.life == ALIVE) {
    Nokia5110_PrintBMP(Explosion.x, Explosion.y, Explosion.image, 0);
    // Explosion disappears after one frame
    Explosion.life = DEAD;
  }
  if(enemy_posture == CLOSE) {
  enemy_posture = OPEN;
  } else { 
    enemy_posture = CLOSE;
  }

  Nokia5110_DisplayBuffer();      // Update the display with information in display buffer screen[].
}

// // Control screen refresh rate. 
// void SysTick_Handler(void){
// 	// signal time to refresh
  
// }

// void GPIOPortF_Handler(void){    // called on release of either SW1 or SW2
// 	// take care of button debounce
	
// 	// SW1: shoot a bullet if there is none.

  
// 	// SW2: start the game, change the game status to ON
// }

// Delay function used for game over prompt timing control: 2s
// To Do: modify the time constant used in the code to generate approximately 2s delay.
void Delay100ms(unsigned long count){
  unsigned long volatile time;
  while(count>0){
    time = 72724;  // 2
    while(time){
	  	time--;
    }
    count--;
  }
}

void GameTick(void){
  TickCount10Hz++;
  time_to_draw = true;
}
