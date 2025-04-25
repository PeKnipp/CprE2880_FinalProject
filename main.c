#include "Timer.h"
#include "lcd.h"
#include "ping.h"  // For scan sensors
#include "servo.h"
#include "open_interface.h"
#include "movement.h"
#include "adc.h" //for IR sensors
#include "scan.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <uart.h>
#include "button.h"
#include <objects.h>

int main(void)
{
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    button_init();
    uart_init();
    adc_init();
    ping_init();
    servo_init();
    scan(0,SONAR);
    timer_waitMillis(200);

    /*--------------------------------------CALIBRATION--------------------------------------*/

        /*----------SERVO----------*/
        //shows the left and right calibration values
        //TODO for each CyBot: assign true_0 and true_180 values in servo.c
    //    servo_calibrate();

        /*----------ADC----------*/
        //TODO for each CyBot: assign coeff and power values in scan.c
    //    scan(90,SONAR);
    //    adc_calibrate();

        /*---------------------------------------------------------------------------------------*/


    //NEEDS iROBOT POWER
    oi_t *sensor_data = oi_alloc(); //Always initalize
    oi_init(sensor_data); //Always initalize

    char scanType;
    scanType = 'i';
    uart_sendStr("\n\rReady to scan: IR Sensor\n\n\r"); //indicates the bot is ready to scan
    distance_threshold = 710; //reassigning value since the IR sensors values are different
    degree_increment = 1;

    while (1)
    {
        detect_objects(sensor_data, scanType, distance_threshold);
        smallest_object();
        print_objects();
        drive_smallest_object();
        change_parameters();
        if (command_byte == '\e')
        {
            uart_sendStr("\n\rPProgram terminated.\n\n\r");
            break;
        }
}

    oi_free(sensor_data);
}
