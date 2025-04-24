/*
 * movement.h
 *
 *  Created on: Feb 7, 2025
 *      Author: pknipper
 */

#ifndef LAB2_CODE_FILES_MOVEMENT_H_
#define LAB2_CODE_FILES_MOVEMENT_H_

#include "open_interface.h"
#include "lcd.h"

double move_forward(oi_t *sensor_data, double distance_mm);

double move_backward(oi_t *sensor_data, double distance_mm);

double turn_right(oi_t *sensor, double degrees);

double turn_left(oi_t *sensor, double degrees);

double bump_right(oi_t *sensor);

double bump_left(oi_t *sensor);

#endif /* LAB2_CODE_FILES_MOVEMENT_H_ */
