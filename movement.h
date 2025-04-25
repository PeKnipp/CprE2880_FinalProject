/*
 * movement.h
 *
 *  Created on: Feb 6, 2025
 *      Author: mansa
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"



double move_forward(oi_t *sensor_data, double distance_mm); //func def, connects movement.c to main.c

double move_backward(oi_t *sensor_data, double distance_mm);

double turn_right(oi_t *sensor_data, double degrees);

double turn_left(oi_t *sensor_data, double degrees);

double movement_bumping(oi_t *sensor_data, double distance_mm);

void bump(int dir);

void hole(int reading, int dir);




#endif /* MOVEMENT_H_ */
