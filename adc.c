/*
 * adc.c
 *
 *  Created on: Mar 25, 2025
 *      Authors: pknipper and csledo
 */


#include "adc.h"
#include "timer.h"
#include "lcd.h"
#include "stdio.h"
#include "uart-interrupt.h"

void adc_init(){
    //Start Port B
    SYSCTL_RCGCGPIO_R |= 0x02;

    //Start ADC0
    SYSCTL_RCGCADC_R |= 0x01;

    //Check active
    while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
    while ((SYSCTL_PRADC_R & 0x01) == 0) {};

    GPIO_PORTB_DIR_R &= 0xEF;
    GPIO_PORTB_AFSEL_R |= 0x10;
    GPIO_PORTB_DEN_R &= 0xEF;
    GPIO_PORTB_AMSEL_R |= 0x10;

    ADC0_ACTSS_R &= 0xFFF7;
    ADC0_EMUX_R &= 0x0FFF;
    ADC0_SSMUX3_R &= 0xFFF0;
    ADC0_SSMUX3_R += 10;
    ADC0_SSCTL3_R |= 0x0006;
    ADC0_IM_R &= 0xFFF7;
    ADC0_ACTSS_R |= 0x0008;
    ADC0_SAC_R |= 0x04; //hardware averaging: 16x hardware oversampling
}

uint16_t adc_read(){
    uint16_t result;
    ADC0_PSSI_R = 0x0008; // 1) initiate SS3
    while((ADC0_RIS_R&0x08)==0){}; // 2) wait for conversion done
    result = ADC0_SSFIFO3_R & 0xFFF; // 3) read result
    ADC0_ISC_R = 0x0008; // 4) acknowledge completion
    return result;
}

void adc_calibrate(){
    char output[50];
    char result[50];
    char valuesY[50];
    char valuesX[50];
    int distance;
    int i = 0;
    int yVals[10];
    uart_sendStr("\n\rTaking ADC calibration values\n\r\n\r");
    for(distance = 5; distance <= 50; distance = distance){
        //take sample of analog input
        int sample = adc_read();
        yVals[i] = sample;
        //delay for sample/display
        timer_waitMillis(300);
        //print to display
        sprintf(output, "ADC value for %d cm: \n%d", distance, sample); //%f , estimation
        lcd_printf(output);
        if(command_flag == 11){
            sprintf(result, "VVoltage value for %d cm: %d\n\r", distance, sample); //%f , estimation
            uart_sendStr(result);
            i++;
            distance += 5;
            command_flag = 0;
        } //wait for confirmation in PUTTY
   }
    uart_sendStr("Finished calibration\n\r\n\r");
    uart_sendStr("Formatted output:\n\r");
    for (distance = 5; distance <= 50; distance += 5)
    {
        sprintf(valuesX, "%d ", distance);
        uart_sendStr(valuesX);
    }
    uart_sendStr("\n\r");

    for (i = 0; i < 10; ++i)
    {
        sprintf(valuesY, "%d ", yVals[i]);
        uart_sendStr(valuesY);
    }
    uart_sendStr("\n\r");
}
