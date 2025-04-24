/**
 * scan.c: Functions for moving the sensor towards a specified angle and taking a reading from one of the sensors
 *
 *  @author Christopher Ledo
 *  @date 4/18/2025
 *
 *
 */


#ifndef SCAN_H_
#define SCAN_H_

#include <servo.h>
#include <ping.h>
#include <adc.h>
#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "Timer.h"

/**
 * Takes a scan at the given angle of type scanType
 */
double scan(int angle, int scanType);

#endif /* SCAN_H_ */
