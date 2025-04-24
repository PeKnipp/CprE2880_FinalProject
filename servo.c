/*
 * servo.c
 *
 *  Created on: Apr 8, 2025
 *      Author: pknipper
 */
#include "servo.h"

/*--------------------------------------CALIBRATION--------------------------------------*/
volatile uint32_t true_0 = 0x4C587;
volatile uint32_t true_180 = 0x45E67;
volatile uint8_t calibrated = 1; //set to 1 if calibrated values are set
/*---------------------------------------------------------------------------------------*/



void servo_init(void){
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x02;

    while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
    while ((SYSCTL_PRTIMER_R & 0x02) == 0) {};

    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R &= ~0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x00700000;

    TIMER1_CTL_R &= ~0x100;
    TIMER1_CFG_R |= 0x04;
    TIMER1_TBMR_R |= 0x0A;

    unsigned long pwm_period = 0x4E200;
    TIMER1_TBILR_R &= 0x0;
    TIMER1_TBILR_R = pwm_period & 0x0FFFF;
    TIMER1_TBPR_R &= 0x0;
    TIMER1_TBPR_R = pwm_period >> 16;
    TIMER1_TBMATCHR_R &= 0x0;
    TIMER1_TBPMR_R &= 0x0;


    TIMER1_CTL_R |= 0x100;
    }


void servo_move(int16_t degrees){
    long match_value_period;
    long match_value_shifted;
    if(!calibrated){
        float match_value_ms = 20.0 - (degrees/180.0+1.0);
        match_value_period = (match_value_ms / 1000)/0.0000000625;
    }
    else{
        match_value_period = (((signed int)(true_180 - true_0))/180) * degrees + true_0;
    }

    match_value_shifted = match_value_period >> 16;
//    TIMER1_TBMATCHR_R &= 0x0;
//    TIMER1_TBPMR_R &= 0x0;
    TIMER1_TBMATCHR_R = match_value_period & 0x0FFFF;
    TIMER1_TBPMR_R = match_value_shifted;

}

void servo_calibrate(void){
    uint8_t button = 0;
    int16_t angle = 0;
    uint32_t match_value;
    char info[100];

    servo_move(angle);

    while(button != 4){
        match_value = (TIMER1_TBPMR_R << 16) + TIMER1_TBMATCHR_R;

        sprintf(info, "<- B1 [B4 Set] B2 ->%X | Set 0", match_value);
        lcd_clear();
        lcd_puts(info);

        button = button_getButton();

        if(button == 1){
            angle++;
            servo_move(angle);
        }
        else if(button == 2){
            angle--;
            servo_move(angle);
        }
        timer_waitMillis(100);
    }
    true_0 = match_value;
    angle = 180;
    servo_move(angle);
    button = 0;
    while(button != 4){
        match_value = (TIMER1_TBPMR_R << 16) + TIMER1_TBMATCHR_R;
        sprintf(info, "<- B1 [B4 Set] B2 ->%X | Set 180", match_value);
        lcd_clear();
        lcd_puts(info);

        button = button_getButton();

        if(button == 1){
            angle++;
            servo_move(angle);
        }
        else if(button == 2){
            angle--;
            servo_move(angle);
        }
        timer_waitMillis(100);
    }
    true_180 = match_value;
    calibrated = 1;
    servo_move(90);

}
