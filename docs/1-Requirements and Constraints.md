# Requirements & Constraints

## Description
A design and implemented Space Invaders-style game on an embedded system using the TM4C123 LaunchPad platform.

### Controls
- Sw2 (right switch): Start the game
- Sw1 (left switch): Fire a bullet
- Potentiometer: Controls horizontal movement of the spaceship

### Gameplay Mechanics
The space ship is positioned at **the bottom of the screen** and moves horizontally within the range of 0 to 83. Enemies move **left to right** at a speed of **1 pixel every 0.1 seconds.** There is only either zero or one bullet that exist at any given time.

#### Bullet Behavior:
The bullet is fired from the middle top of the spaceship and moved upward at **1 pixel every 0.1 seconds.** 
The bullet disappears when
- It hits an enemy, or
- It reaches at the top of the screen 

### Enemy Behavior
An enemy is eliminated when it is either hit by a bullet, or it reaches at the right edge of the screen.

### Game Flow
1. Display the start screen
2. Press SW2 to begin the game
3. Enemies move across the screen
4. Player shoots enemies using SW1
5. Game ends when all enemies are eliminated
6. Display **game over screen** for 3 second
7. return to start screen

### Sprites Required
**What is sprites?**
In computer games, a **sprite** is a **2D image** or **animation** that represents an object in the game world.
**What components to the sprite that we must include**
- At least **3 different enemy designs**
- A **spaceship**
- A **bullet**
- An **explosion animation**

## Hardware Requirements
- TM4C123GH6PM LaunchPad Development Board
- 1 Potentiometer
- 2 Switches (SW1, Sw2) onboard
- 2 LEDs onboard
- 1 Nokia 5110 LCD
- Resistors (10k Ohm, 5k Ohm, 100 Ohm)
- 0.1 uF capacitor
- Components for 4-bit R-2R DAC
- Speaker/Buzzer and LM386 audio amplifier

## System Requirements
- [ ] RQ-01 PLL configuration
    - 16 MHz < System clock < 80MHz
- [ ] RQ-02 Hardware timer interrupt
    - 10Hz
    - Game timing for 3 second end prompt
    - Screen updates every 0.1 second 
- [ ] RQ-03 Interrupt-based switch handling
    - SW1 Press for fire
    - SW2 Starts the game
- [ ] RQ-04 ADC for potentiometer input
    - Moves from 0 to 83
- [ ] RQ-05 SSI interface for Nokia 5110 LCD
    - 10k Ohms: SCLK, DN, D/C, RST
    - 1K Ohms: SCE
- [ ] RQ-06 Collision detection logic
    - Collision is detected when bounding box of a bullet overlaps the bound box of an enemy sprite
- [ ] RQ-07 Score tracking and display
    - Score increments by 1 per enemy eliminated and is displayed on game-over screen
    - Real-time game rendering

## Functional Requirements
- [ ] RQ-08 Movement
    - Enemy animation moves 1 pixel per 0.1 seconds
    - Bullet movement from spaceship goes up for 1 pixel per 0.1 seconds
- [ ] RQ-10 Shooting system
    - One active bullet at a time
    - Bullet enemy collision detection
    - Explosion animation up hit
        - Displays for 0.1s after bullet hits an enemy
        - Explosion image will be displayed in place of the hit enemy to replace enemy image
- [ ] RQ-11 Sound
    - 4-bit R-2R DAC
    - Sound effects for shooting, enemy hit
    - Hardware timer for sound timing

## Constraints
- [ ] CN-01 ADc Module for potentiometer input
    - ADC0 SS3 and ADC0 SS2 is not allowed
- [ ] CN-02 SSI interface for Nokia 5110
    - SSI0 is not allowed
- [ ] CN-03 movement
    - x-range constrained to 0-83 pixels