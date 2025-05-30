/*
 *
 *   uart.c
 *
 *
 *
 *   @author
 *   @date
 */

// The "???" placeholders should be the same as in your uart.c file.
// The "?????" placeholders are new in this file and must be replaced.
#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <uart.h>

// These variables are declared as examples for your use in the interrupt handler.
volatile char command_byte = '\0';
volatile char command_byte_GO = 'M'; // byte value for special character used as a command
volatile char command_byte_STOP = 'h';
volatile char command_byte_QUIT = 'q';
volatile char command_byte_MANUAL = 'm';
volatile char command_byte_FORWARD = 'w';
volatile char command_byte_BACKWARD = 's';
volatile char command_byte_LEFT = 'a';
volatile char command_byte_RIGHT = 'd';
volatile char command_byte_HALT = ' ';
volatile char command_byte_ESCAPE = '\e'; //added by Chris 4/22
volatile char command_byte_PRINT = 'p';
volatile char command_byte_CHANGE = 'c';
volatile char command_byte_THRESHOLD = 't';
volatile int command_flag = 0; // flag to tell the main program a special command was received

void uart_init(void)
{
    //TODO
    //enable clock to GPIO port B
    SYSCTL_RCGCGPIO_R |= 0x02;

    //enable clock to UART1
    SYSCTL_RCGCUART_R |= 0x02;

    //wait for GPIOB and UART1 peripherals to be ready
    while ((SYSCTL_PRGPIO_R & 0x02) == 0)
    {
    };
    while ((SYSCTL_PRUART_R & 0x02) == 0)
    {
    };

    //enable alternate functions on port B pins
    GPIO_PORTB_AFSEL_R |= 0x03;

    //enable digital functionality on port B pins
    GPIO_PORTB_DEN_R |= 0x03;

    //enable UART1 Rx and Tx on port B pins
    GPIO_PORTB_PCTL_R &= 0xEE;     // Force 0's in the desired locations
    GPIO_PORTB_PCTL_R |= 0x11;

    //calculate baud rate
    uint16_t iBRD = 8; //use equations
    uint16_t fBRD = 44; //use equations

    //turn off UART1 while setting it up
    UART1_CTL_R &= 0xFE;

    //set baud rate
    //note: to take effect, there must be a write to LCRH after these assignments
    UART1_IBRD_R = iBRD;
    UART1_FBRD_R = fBRD;

    //set frame, 8 data bits, 1 stop bit, no parity, no FIFO
    //note: this write to LCRH must be after the BRD assignments
    UART1_LCRH_R |= 0b01100000;
    UART1_LCRH_R &= 0b01100001;

    //use system clock as source
    //note from the datasheet UARTCCC register description:
    //field is 0 (system clock) by default on reset
    //Good to be explicit in your code
    UART1_CC_R = 0x0;

    //////Enable interrupts

    //first clear RX interrupt flag (clear by writing 1 to ICR)
    UART1_ICR_R |= 0b00010000;

    //enable RX raw interrupts in interrupt mask register
    UART1_IM_R |= 0x10;

    //NVIC setup: set priority of UART1 interrupt to 1 in bits 21-23
    //NVIC_PRI1_R = (NVIC_PRI1_R & 0xFF0FFFFF) | 0x00200000;
    NVIC_PRI1_R &= 0xFF0FFFFF;
    NVIC_PRI1_R |= 0x00200000;

    //NVIC setup: enable interrupt for UART1, IRQ #6, set bit 6
    NVIC_EN0_R |= 0x40;

    //tell CPU to use ISR handler for UART1 (see interrupt.h file)
    //from system header file: #define INT_UART1 22
    IntRegister(INT_UART1, UART1_Handler);

    //globally allow CPU to service interrupts (see interrupt.h file)
    IntMasterEnable();

    //re-enable UART1 and also enable RX, TX (three bits)
    //note from the datasheet UARTCTL register description:
    //RX and TX are enabled by default on reset
    //Good to be explicit in your code
    //Be careful to not clear RX and TX enable bits
    //(either preserve if already set or set them)
    UART1_CTL_R |= 0x0301;

}

void uart_sendChar(char data)
{
    while (UART1_FR_R & 0x20)
    {
    }

    UART1_DR_R = data;
}
//
//char uart_receive(void){
//	//DO NOT USE this busy-wait function if using RX interrupt
//}
//

//TODO for reference see lcd_puts from lcd.c file
void uart_sendStr(const char *data)
{
    while (*data != '\0')
    {
        uart_sendChar(*data);
        data++;
    }
}

char uart_receive_nonblocking(void)
{
    //DO NOT USE this busy-wait function if using RX interrupt
//    if (UART1_FR_R & 0x40 == 0){
    char data = 0;

    //mask the 4 error bits and grab only 8 data bits
    data = (char) (UART1_DR_R & 0xFF);

    return data;
}

// Interrupt handler for receive interrupts
void UART1_Handler(void)
{
    //check if handler called due to RX event
    if (UART1_MIS_R & UART1_IM_R)
    {
        //byte was received in the UART data register
        //clear the RX trigger flag (clear by writing 1 to ICR)
        UART1_ICR_R |= 0b00010000;

        //read the byte received from UART1_DR_R and echo it back to PuTTY
        //ignore the error bits in UART1_DR_R
        command_byte = uart_receive_nonblocking(); //variable storing bytes received
        uart_sendChar(command_byte);

        //if byte received is a carriage return
        if (command_byte == '\r')
        {
            //send a newline character back to PuTTY
            uart_sendChar('\n');
        }
        else
        {
            //AS NEEDED
            //code to handle any other special characters
            //code to update global shared variables
            //DO NOT PUT TIME-CONSUMING CODE IN AN ISR

            if (command_byte == command_byte_GO)
            {
                command_flag = 1;
            }
            if (command_byte == command_byte_STOP)
            {
                command_flag = 2;
            }
            if (command_byte == command_byte_QUIT)
            {
                command_flag = 3;
            }
            if (command_byte == command_byte_MANUAL)
            {
                command_flag = 5; //4 used by bump
            }
            if (command_byte == command_byte_FORWARD)
            {
                command_flag = 6;
            }
            if (command_byte == command_byte_BACKWARD)
            {
                command_flag = 7;
            }
            if (command_byte == command_byte_LEFT)
            {
                command_flag = 8;
            }
            if (command_byte == command_byte_RIGHT)
            {
                command_flag = 9;
            }
            if (command_byte == command_byte_HALT)
            {
                command_flag = 10;
            }
            if (command_byte == command_byte_ESCAPE)
            {
                command_flag = 11;
            }
            if (command_byte == command_byte_PRINT)
            {
                command_flag = 12;
            }
        }
    }
}
