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

volatile const int SONAR = 0;
volatile const int IR = 1;

double scan(int angle, int scanType){
    /*--------------------------------------CALIBRATION--------------------------------------*/
//    double num1 = 472871; //found using adc_calibrate
//    double num2 = 1.4462; //found using adc_calibrate
    /*---------------------------------------------------------------------------------------*/

    double distance = 0;
    servo_move(angle); //moves the sensor to take a reading at the given angle value
    if(scanType == SONAR){ //takes a reading using the ping sensor
        distance = ping_getDistance(); //getDistance does all calculations internally to find the distance to object
    }
    else if (scanType == IR){ //takes a reading using the IR sensor
         distance = adc_read();
    }
    return distance; //returns distance estimation from the scan
}
