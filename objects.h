/*
 * communication.h
 *
 *  Created on: Feb 11, 2025
 *      Author: mansa
 */

#ifndef OBJECTS_H_
#define OBJECTS_H_
#include "open_interface.h"
#include "uart.h"
#include "servo.h"
#include "scan.h"

extern volatile double distance_threshold;
extern volatile int degree_increment;

void lcd_input();

void ping_sensor(oi_t *sensor_data);

void detect_objects(oi_t *sensor_data, char scanType, double distance_threshold);

void print_objects();

void smallest_object();

void print_string(char input[]);

void calibrate_servo();

void drive_smallest_object();

void change_parameters();

#endif /* OBJECTS_H_ */
