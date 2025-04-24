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

extern volatile const int DISTANCE;
extern volatile const int ANGLE;
extern volatile const int BUMP;

extern volatile const int HOLE;
extern volatile const int BOUNDARY;
extern volatile const int END;

extern volatile const int FAR_LEFT;
extern volatile const int LEFT;
extern volatile const int RIGHT;
extern volatile const int FAR_RIGHT;

/**
 * Takes a scan at the given angle of type scanType
 */
void uart_sendData(int dataType, int dataVal);
void uart_sendHole(int holeType, int holeDir);

#endif /* SCAN_H_ */
