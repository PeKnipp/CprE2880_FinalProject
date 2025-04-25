/*
 * movement.h
 *
 *  Created on: Feb 6, 2025
 *      Author: mansa
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"



void turn_right(oi_t *sensor_data, double degrees);

void turn_left(oi_t *sensor_data, double degrees);

void movement(oi_t *sensor_data);

char hazards(oi_t *sensor_data);

void bump(int dir);

void hole(int dir);



#endif /* MOVEMENT_H_ */
