/*
 * servo.h
 *
 *  Created on: Apr 8, 2025
 *      Author: pknipper
 */

#ifndef LAB9_CODE_FILES_SERVO_H_
#define LAB9_CODE_FILES_SERVO_H_

#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "button.h"
#include "lcd.h"
#include "timer.h"

void servo_init(void);
void servo_move(int16_t degrees);
void servo_calibrate(void);

#endif /* LAB9_CODE_FILES_SERVO_H_ */
