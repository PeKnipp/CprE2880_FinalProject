/**
 * scan.c: Functions for moving the sensor towards a specified angle and taking a reading from one of the sensors
 *
 *  @author Christopher Ledo
 *  @date 4/18/2025
 */

#include <servo.h>
#include <ping.h>
#include <adc.h>
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "Timer.h"

/**
 * A replacement for the cyBOT_Scan function from previous labs
 */

//typedef struct
//{
//    int ping;
//    float ir;
//}scandata;

volatile const int SONAR = 0;
volatile const int IR = 1;

double scan(int angle, int scanType){
    /*--------------------------------------CALIBRATION--------------------------------------*/
//    double num1 = 229.3995; //found using adc_calibrate
//    double num2 = 27.9881; //found using adc_calibrate
    /*---------------------------------------------------------------------------------------*/

    double distance = 0;
    servo_move(angle); //moves the sensor to take a reading at the given angle value
    if(scanType == SONAR){ //takes a reading using the ping sensor
        distance = ping_getDistance(); //getDistance does all calculations internally to find the distance to object
    }
    else if (scanType == IR){ //takes a reading using the IR sensor
//        distance = pow((num1/adc_read()), num2); //calculations to use the ADC voltage to estimate distance
        distance = adc_read();
//                num1 - (num2*log(adc_read()));
    }
    return distance; //returns distance estimation from the scan
}
