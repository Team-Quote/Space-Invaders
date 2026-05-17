# System Design & Module Planning
## Description
### Purpose
To explore multiple design approaches and establish a modular architecture.
### What are we doing
We propose a two distinct design alternatives. This includes a hardware plan and software implementation method for each. We will then compare the designs and select the best solution. This includes a complete module design and assignment table.
## Design Option A
A layered modular architecture with separate hardware drivers, device, service modules, and game
### Core Hardware
- TM4C123GH6PM LaunchPad Development Board
- 1 Potentiometer
- 2 Switches (SW1, Sw2) onboard
- 2 LEDs onboard
- 1 Nokia 5110 LCD
- Resistors (10k Ohm, 5k Ohm, 100 Ohm)
- 0.1 uF capacitor
- Components for 4-bit R-2R DAC
- Speaker/Buzzer and LM386 audio amplifiers
#### Pin Plan
**Potentiometer output**
- PE3 / AIN0 using ADC0 SS1
ADC0 SS2 and SS3 are not allowed
**Switches**
- SW1 fire = PF4
- SW2 start = PF0
**LEDs**
- R
- G
- B
**LCD Nokia Display**
Using SSI2
- RST, pin 1 = PC5
	- 10 k ohm resistor
- CE, pin 2 = PB5
	- 1 k ohm resistor
- DC, Pin 3 = PC4
	- 10 k ohm resistor
- Din, Pin 4 = PB7
	- 10 k ohm resistor 
- CLK, Pin 5 = PB4
	- 10 k ohm resistor
- Vcc, Pin 6 = 3.3V
- BL, Pin 7 = N/A
- Gnd, Pin 8 = Ground
**4-bit DAC**
Use one 4-bit GPIO nibble on PB0-PB3
- PB0 = DAC bit 0
- PB1 = DAC bit 1
- PB2 = DAC bit 2
- PB3 = DAC bit 3
PB0-PB3 -> 4-bit R-2R ladder -> LM386 -> speaker
**Timers**
- Timer1A = 10 Hz periodic game update interrupt
- Timer0A = high-rate periodic interrupt for sound sample output
**Audio Output**
- 4-bit R-2R ladder DAC
- LM386 amplifier
- Speaker
### Software
#### Architecture
**Top-level**
`SpaceInvaders.c`
- Main Integration file
`ModuleTest.c`
- Standalone module test harness for driver and feature verification before system integration
**Hardware Layer**
`PLL.c / PLL.h`
`Nokia5110.c / Nokia5110.h`
`ADC0SS1.c / ADC0SS1.h`
`Timer0.c / Timer0.h`
`Timer1A.c / Timer1A.h`
`DAC.c / DAC.h`
**Feature Layer**
`Sound.c / Sound.h`
`SW.c / SW.h`
`LED.c / LED.h`
## Design Option B
This design follows the same basic hardware, but organizes the software to a more centralized game controller file and fewer modules
### Core Hardware
- TM4C123GH6PM LaunchPad Development Board
- 1 Potentiometer
- 2 Switches (SW1, Sw2) onboard
- 2 LEDs onboard
- 1 Nokia 5110 LCD
- Resistors (10k Ohm, 5k Ohm, 100 Ohm)
- 0.1 uF capacitor
- Components for 4-bit R-2R DAC
- Speaker/Buzzer and LM386 audio amplifiers
#### Pin Plan
**Potentiometer output**
- PE3 / AIN0 using ADC0 SS1
ADC0 SS2 and SS3 are not allowed
**Switches**
- SW1 fire = PF4
- SW2 start = PF0
**LEDs**
- R
- G
- B
**LCD Nokia Display**
Using SSI2
- RST, pin 1 = PC5
	- 10 k ohm resistor
- CE, pin 2 = PB5
	- 1 k ohm resistor
- DC, Pin 3 = PC4
	- 10 k ohm resistor
- Din, Pin 4 = PB7
	- 10 k ohm resistor 
- CLK, Pin 5 = PB4
	- 10 k ohm resistor
- Vcc, Pin 6 = 3.3V
- BL, Pin 7 = N/A
- Gnd, Pin 8 = Ground
**4-bit DAC**
Use one 4-bit GPIO nibble on PB0-PB3
- PB0 = DAC bit 0
- PB1 = DAC bit 1
- PB2 = DAC bit 2
- PB3 = DAC bit 3
PB0-PB3 -> 4-bit R-2R ladder -> LM386 -> speaker
**Timers**
- Timer1A = 10 Hz periodic game update interrupt
- Timer0A = high-rate periodic interrupt for sound sample output
**Audio Output**
- 4-bit R-2R ladder DAC
- LM386 amplifier
- Speaker
### Software
#### Architecture
**Top-level**
`SpaceInvaders.c`
- Main Integration file
- Owns the game state machine
- Contains `System_Init()`, `Start_Prompt()`, `Game_Init()`, `Move()`, `Draw()`, `End_Prompt()`, and `GameTick()`
- Stores sprite bitmap tables and the active game-object state
`ModuleTest.c`
- Standalone module test harness for driver and feature verification before system integration
**Hardware Layer**
`PLL.c / PLL.h`
`Nokia5110.c / Nokia5110.h`
`ADC0SS1.c / ADC0SS1.h`
`Timer0.c / Timer0.h`
`Timer1A.c / Timer1A.h`
`DAC.c / DAC.h`
**Feature Layer**
`Sound.c / Sound.h`
`SW.c / SW.h`
`LED.c / LED.h`
## Module Design & Assignment Table

| Module Name | Responsibility | Assigned Team Member |
| ----------- | -------------- | -------------------- |
|             |                |                      |
