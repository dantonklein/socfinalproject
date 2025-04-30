#pragma once
#include <stdio.h>
#include "xil_io.h"
#include "xparameters.h"


/************************** Constant Definitions *****************************/

#define GPIO_ONE  XPAR_XGPIO_0_BASEADDR 
#define BIT(x) (1<<x)
// #define DEBUG_PROGRAM 

// Pong macros
#define GAME_START 0b10000
#define PADDLE_UP_P2 BIT(2)
#define PADDLE_DOWN_P2 BIT(3)
#define X_MAX 38 // orig. 40
#define Y_MAX 29 // orig. 30
#define PADDLE_LENGTH (Y_MAX / 4) // Approx

// PWM macros
#define PWM_LOW 0x00
#define PWM_HIGH 0xFF

// External memory macros

// Tone 1 - Paddle Hit
#define TONE1_LENGTH 2000
#define TONE1_START_ADDR XPAR_XBRAM_0_BASEADDR
#define TONE1_END_ADDR (TONE1_START_ADDR + TONE1_LENGTH - 1)

// Tone 2 - Score
#define TONE2_LENGTH 2000
#define TONE2_START_ADDR (TONE1_END_ADDR + 1)
#define TONE2_END_ADDR (TONE2_START_ADDR + TONE2_LENGTH - 1)

// Song - Mary had a little lamb
#define SONG_LENGTH 41600
#define SONG_START_ADDR (TONE2_END_ADDR + 1)
#define SONG_END_ADDR (SONG_START_ADDR + SONG_LENGTH - 1)


// For testing only
extern const uint8_t sine_wave[256];