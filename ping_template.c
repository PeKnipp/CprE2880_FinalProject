/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping_template.h"
#include "Timer.h"

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile enum{LOW, HIGH, DONE} g_state = LOW; // State of ping echo pulse
volatile unsigned long last_time = 0;
volatile unsigned long current_time = 0;
volatile char update_flag = 0;
volatile unsigned long time_diff;
volatile unsigned short overflow = 0;

void ping_init (void){

  // YOUR CODE HERE
    //Start Ports B and F
    SYSCTL_RCGCGPIO_R |= 0x22;

    //Start Timer 3
    SYSCTL_RCGCTIMER_R |= 0x08;

    //Check active
    while ((SYSCTL_PRGPIO_R & 0x22) == 0) {};
    while ((SYSCTL_PRTIMER_R & 0x08) == 0) {};

    GPIO_PORTB_DEN_R |= 0x08;
    GPIO_PORTB_DIR_R &= ~0x08;
//    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_PCTL_R |= 0x07000;

    TIMER3_CTL_R &= ~0x100;
    TIMER3_CTL_R |= 0xC00;
    TIMER3_CFG_R |= 0x04;
    TIMER3_TBMR_R |= 0x07;
    TIMER3_TBILR_R |= 0xFFFF;
    TIMER3_TBPR_R |= 0xFF;
    TIMER3_IMR_R |= 0x0400;
    TIMER3_ICR_R |= 0x0400;


    NVIC_PRI9_R |= 0x20;
    NVIC_EN1_R |= 0x10;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();

    // Configure and enable the timer
    TIMER3_CTL_R |= 0x01;
}

void ping_trigger (void){
    g_state = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x100;
    TIMER3_IMR_R &= ~0x400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= 0xF7;

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    GPIO_PORTB_DEN_R |= 0x08;
    GPIO_PORTB_DIR_R |= 0x08;


    GPIO_PORTB_DATA_R &= 0xF7;
    GPIO_PORTB_DATA_R |= 0x08;

    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= 0xF7;


    GPIO_PORTB_DIR_R &= ~0x08;
    GPIO_PORTB_AFSEL_R |= 0x08;
    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x404;
    // Re-enable alternate function, timer interrupt, and timer

    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0x100;
}

void TIMER3B_Handler(void){

  // YOUR CODE HERE
  // As needed, go back to review your interrupt handler code for the UART lab.
  // What are the first lines of code in the ISR? Regardless of the device, interrupt handling
  // includes checking the source of the interrupt and clearing the interrupt status bit.
  // Checking the source: test the MIS bit in the MIS register (is the ISR executing
  // because the input capture event happened and interrupts were enabled for that event?
  // Clearing the interrupt: set the ICR bit (so that same event doesn't trigger another interrupt)
  // The rest of the code in the ISR depends on actions needed when the event happens.
    if (TIMER3_MIS_R == 0x400){
        TIMER3_ICR_R |= 0x404;
        last_time = current_time;
        current_time = TIMER3_TBR_R;
        //update_flag = 1;


        if(current_time > last_time){
            overflow ++;
        }
        time_diff = fabs(last_time - current_time);
        //update_flag = 0;
    }

}

float ping_getDistance (void){
    float distance =  (time_diff * 0.00000625 * 343.0)/2.0;
    return distance;

}
