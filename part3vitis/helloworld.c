/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * main.c: implements player vs computer pong, with sound
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "xparameters.h"
#include "xil_io.h"

#include "xgpio.h"
#include "sleep.h"

#include "constants.h"
#include "init.h"

uint8_t old_p1_score = 0;
uint8_t old_p2_score = 0;
#define WIN_SCORE 9  // Winning score

/************************** Variable Definitions *****************************/
XGpio Gpio1; /* The Instance of the GPIO Driver */
XAxiCdma_Config cdma_config; /* For CDMA */

int main()
{
    // Initializations
    init_platform();
    //init_CDMA(&cdma_config);
    init_system(&Gpio1); // System failure dealt with internally

// Test LEDs
#if 0
    u32 led_write = 0xAAAAAAAA;
    for (int i = 0; i < 8; i++) {
        // Write to LEDs, rgbA, and rgbB
        for (int j = 0; j < 3; j++) {
            Xil_Out32(XPAR_APB_LED_CTL_0_BASEADDR + (4 * j), led_write);
        }
        led_write = ~led_write;
        sleep(1);
    }
#endif

// Test PWM
#if 0
    for (uint8_t i = 0; ; i++) {
        Xil_Out32(XPAR_AXI_PWM_DRIVER_0_BASEADDR, sine_wave[i]);
        usleep(20); // Adjusts tone
    }
#endif

// Test shared memory + PWM
#if 0
    uint16_t cycle_delay = 200;
    uint16_t tone_delay = 1; 
    while (1) {
        // Play tone 1
        for (uint32_t* addr = (uint32_t*)TONE1_START_ADDR; addr <= (uint32_t*)TONE1_END_ADDR; addr++) {
            Xil_Out32(XPAR_AXI_PWM_DRIVER_0_BASEADDR, *addr);
            usleep(cycle_delay);
        }
        sleep(tone_delay);

        // Play tone 2
        for (uint32_t* addr = (uint32_t*)TONE2_START_ADDR; addr <= (uint32_t*)TONE2_END_ADDR; addr++) {
            Xil_Out32(XPAR_AXI_PWM_DRIVER_0_BASEADDR, *addr);
            usleep(cycle_delay);
        }
        sleep(tone_delay);

        // Play song
        for (uint32_t* addr = (uint32_t*)SONG_START_ADDR; addr <= (uint32_t*)SONG_END_ADDR; addr++) {
            Xil_Out32(XPAR_AXI_PWM_DRIVER_0_BASEADDR, *addr);
            usleep(cycle_delay);
        }
        sleep(tone_delay);
    }
#endif

    int Status = XST_SUCCESS;

    print("-- Starting Game --\r\n");
    sleep(1);

    Status = Xil_SecureOut32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 0), GAME_START);

    if(Status == XST_FAILURE)
    {
        print("Address pong Write failed\r\n");
        while(1);
    }

    uint8_t calculation_complete = 0;
    uint8_t ideal_paddle_pos_p2_bot = 0;

    while (1) {

        uint32_t output_to_pongIP = GAME_START;
        
        // read p1 paddle data from gpio
        uint32_t paddle_pos_p1 = XGpio_DiscreteRead(&Gpio1, 1);

        // add p1 paddle data to be written to pong ip later
        output_to_pongIP |= paddle_pos_p1;

        // Read data from pong ip
        uint32_t slv_reg1_val = Xil_In32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 1));
        uint32_t slv_reg2_val = Xil_In32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 2));
        uint32_t slv_reg3_val = Xil_In32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 3));
        uint32_t slv_reg4_val = Xil_In32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 4));
        uint32_t slv_reg5_val = Xil_In32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 5));
        uint32_t slv_reg6_val = Xil_In32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 6));
        uint32_t slv_reg7_val = Xil_In32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 7));
        uint32_t slv_reg8_val = Xil_In32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 8));

        // Use following variables to determine a win/loss
        uint8_t paddle_pos_p1_bot = slv_reg1_val;
        uint8_t paddle_pos_p1_top = slv_reg2_val;
        uint8_t paddle_pos_p2_bot = slv_reg3_val;
        uint8_t paddle_pos_p2_top = slv_reg4_val;
        uint8_t ball_x = slv_reg5_val >> 1;
        uint8_t ball_dir_x = slv_reg5_val & BIT(0);
        uint8_t ball_y = slv_reg6_val >> 1;
        uint8_t ball_dir_y = slv_reg6_val & BIT(0);
        uint8_t p1_score = slv_reg7_val;
        uint8_t p2_score = slv_reg8_val;

        // For debugging ball position
        //#if 1 
        //xil_printf("paddle pos p1 bot = %i\n\r", paddle_pos_p1_bot);
        //xil_printf("paddle pos p1 top = %i\n\r", paddle_pos_p1_top);
        //xil_printf("paddle pos p2 bot = %i\n\r", paddle_pos_p2_bot);
        //xil_printf("paddle pos p2 top = %i\n\r", paddle_pos_p2_top);
        //xil_printf("ball_x = %i\n\r", ball_x);
        //xil_printf("ball_dir_x = %i\n\r", ball_dir_x);
        //xil_printf("ball_y = %i\n\r", ball_y);
        //xil_printf("ball_dir_y = %i\n\r", ball_dir_y);
        //xil_printf("p1_score = %i\n\r", p1_score);
        //xil_printf("p2_score = %i\n\r\n\n", p2_score);
        //#endif

        xil_printf("ball_x = %d, ball_dir_x = %d, ball_y = %d\n\r", ball_x, ball_dir_x, ball_y);

        // If ball direction is right
        if (ball_dir_x) {
            // Prevents unnessesary calculations
            if (!calculation_complete) {
                
                int magnitude;

                // If ball is moving up
                if (!ball_dir_y) {
                    //xil_printf("Ball bouncing up\n\r");
                    magnitude = ((X_MAX - ball_x) - ball_y) % Y_MAX;
                }
                else {
                    //xil_printf("Ball bouncing down\n\r");
                    magnitude = (ball_y + (X_MAX - ball_x)) % Y_MAX;
                }

                ideal_paddle_pos_p2_bot = Y_MAX - magnitude;

                calculation_complete = 1;
            }
            
        }
        else {
            // Prepare for next ball pass
            calculation_complete = 0;
        }

        // p2 paddle movement
        if ((paddle_pos_p2_bot + 1) < ideal_paddle_pos_p2_bot) {
            output_to_pongIP |= BIT(2);
        }
        else if ((paddle_pos_p2_bot + 1) > ideal_paddle_pos_p2_bot) {
            output_to_pongIP |= BIT(3);
        }

        // Logic for playing tone for a ball hits a paddle, a score or a win/loss

        // --- Task 3 Audio Event Logic ---

        // Paddle Hit Detection
        if ((ball_x == 0 && ball_y <= paddle_pos_p1_top && ball_y >= paddle_pos_p1_bot) ||
        (ball_x == X_MAX && ball_y <= paddle_pos_p2_top && ball_y >= paddle_pos_p2_bot))

        {
        // Play Tone 1 (paddle hit)
        for (uint32_t* addr = (uint32_t*)TONE1_START_ADDR; addr <= (uint32_t*)TONE1_END_ADDR; addr++) {
        Xil_Out32(XPAR_AXI_PWM_DRIVER_0_BASEADDR, *addr);
        usleep(200);
        }
        }


        // Score Detection
        if (p1_score > old_p1_score || p2_score > old_p2_score)
        {
            // Play Tone 2 (score)
            for (uint32_t* addr = (uint32_t*)TONE2_START_ADDR; addr <= (uint32_t*)TONE2_END_ADDR; addr++) {
                Xil_Out32(XPAR_AXI_PWM_DRIVER_0_BASEADDR, *addr);
                usleep(200);
            }
        }

        // Win/Loss Detection
        if (p1_score == WIN_SCORE || p2_score == WIN_SCORE)
        {
            // Play Marry had a little lamb (Win/Loss)
            for (uint32_t* addr = (uint32_t*)SONG_START_ADDR; addr <= (uint32_t*)SONG_END_ADDR; addr++) {
                Xil_Out32(XPAR_AXI_PWM_DRIVER_0_BASEADDR, *addr);
                usleep(200);
            }
        }

        // Update old scores for next loop
        old_p1_score = p1_score;
        old_p2_score = p2_score;

        // --- End of Task 3 Audio Event Logic ---

        // Logic for playing tone for a ball hits a paddle, a score or a win/loss

        // update all pong inputs
        Xil_Out32(XPAR_MYIP_PONG2_0_BASEADDR + (4 * 0), output_to_pongIP);

    }

    print("Exited Error\r\n");
    cleanup_platform();
    return 0;
}