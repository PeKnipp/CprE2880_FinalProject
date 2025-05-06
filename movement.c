/*
 * movement.c
 *
 *  Created on: Feb 6, 2025
 *      Author: mansa
 */

#include "open_interface.h"
#include "movement.h"
#include "gui.h"
#include "uart.h"

volatile char wheels_on = 0;

void move_forward(oi_t *sensor_data, double distance_mm)
{
    double CALIBRATION_VALUE = 1.25;
    distance_mm *= CALIBRATION_VALUE;
    double sum = 0;
    while (sum < distance_mm)
    {
        oi_update(sensor_data);
        hazards(sensor_data);
        oi_setWheels(150, 150);
        while (sum < 10)
        { //runs irobot until the sum data is equal to the specified distance
            oi_update(sensor_data);
            sum += sensor_data->distance; //sends a pointer to distance, stores the value in the temp var sum
            uart_sendData(DISTANCE, 10);
        }

        sum += sensor_data->distance;
//        }
    }
    oi_setWheels(0, 0);
}

void move_backward(oi_t *sensor_data, double distance_mm)
{
    double CALIBRATION_VALUE = 1.25;
    distance_mm *= CALIBRATION_VALUE;
    double sum = 0;
    while (sum > -1 * distance_mm)
    {
        oi_update(sensor_data);
        oi_setWheels(-150, -150);
        while (sum > -10)
        { //runs irobot until the sum data is equal to the specified distance
            oi_update(sensor_data);
            sum += sensor_data->distance; //sends a pointer to distance, stores the value in the temp var sum
            uart_sendData(DISTANCE, 10); //sensor_data->distance
        }

        sum += sensor_data->distance;
    }
    oi_setWheels(0, 0);
}

void turn_right(oi_t *sensor_data, double degrees)
{
    double CALIBRATION_VALUE = 0.7;
    degrees *= CALIBRATION_VALUE;
    double sum = 0;
    oi_setWheels(50, -50); // sets the speed of the left wheel to move backward and the right wheel to move forward, which turns the robot right
    //DEVNOTE: This movement logs the sum value negatively, so the mathematical operations are inverted from the turn_left method.
    while (sum >= -degrees) //turns the irobot right. sum value decriments until it reaches the specified degree value.
    {
        oi_update(sensor_data);
        sum += sensor_data->angle; //sends a pointer to angle, stores the value in the temp var sum
    }
    oi_setWheels(0, 0);
}
void turn_left(oi_t *sensor_data, double degrees)
{
    degrees *= 0.7;
    double sum = 0;
    oi_setWheels(-50, 50); //sets the speed of the right wheel to move forward and the left wheel to move backward, which turns the robot left.
    while (sum < degrees) //turns irobot left. sum value increments until it reaches the specified degree value.
    {
        oi_update(sensor_data);
        sum += sensor_data->angle; //sends a pointer to angle, stores the value in the temp var sum
    }
    oi_setWheels(0, 0); // stops the robot when while loop is finished
}

void movement(oi_t *sensor_data)
{
        char hazard_detected = hazards(sensor_data);
        if (hazard_detected == 1){
            if (wheels_on){
                oi_setWheels(0, 0);
                wheels_on = !wheels_on;
            }
}
        if (command_flag == 6)
        {
            if (!hazard_detected == 1)
            {   //stops forward movement on hazards. can still turn and back up
                if (!wheels_on){
                    oi_setWheels(152, 150);
                    wheels_on = !wheels_on;

                }
                oi_update(sensor_data);
            }
            uart_sendData(DISTANCE, sensor_data->distance);
        }
        if (command_flag == 7)
        {
            if (!wheels_on){
                oi_setWheels(-152, -150);
                wheels_on = !wheels_on;

            }
            oi_update(sensor_data);
            uart_sendData(DISTANCE, sensor_data->distance);
        }
        if (command_flag == 8)
        {
            turn_left(sensor_data, 15);
            uart_sendData(ANGLE, sensor_data->angle);
            command_flag = 0;
        }
        if (command_flag == 9)
        {
            turn_right(sensor_data, 15);
            uart_sendData(ANGLE, sensor_data->angle);
            command_flag = 0;
        }
        if (command_flag == 10)
        {
            if (wheels_on){
                oi_setWheels(0, 0);
                wheels_on = !wheels_on;
            }
        }

}

char hazards(oi_t *sensor_data) //once roomba bumps, gets called, will retreat, turn, go forward, turn, and subtract the sum value from the distance_mm value to compensate for distance traveled when doing the turning cycle
{
    int reading;
    oi_update(sensor_data);
    int lcliff_v = sensor_data->cliffLeftSignal;
    int f_lcliff_v = sensor_data->cliffFrontLeftSignal;
    int f_rcliff_v = sensor_data->cliffFrontRightSignal;
    int rcliff_v = sensor_data->cliffRightSignal;
    lcd_printf("lcliff_v = %d\n\rf_lcliff_v = %d\n\rf_rcliff_v = %d\n\rrcliff_v = %d", lcliff_v,
                       f_lcliff_v, f_rcliff_v, rcliff_v); //for testing
    //TODO: calibrate based on bot
    int hole_threshold = 200;//bot 7
    int tape_threshold = 2100;//bot 12

    char hazard_detected = 0;

    if (sensor_data->bumpLeft)
    {
        bump(LEFT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (sensor_data->bumpRight)
    {
        bump(RIGHT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (lcliff_v < hole_threshold)
    {
        reading = lcliff_v;
        hole(reading, FAR_LEFT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (lcliff_v > tape_threshold)
    {
        reading = lcliff_v;
        hole(reading, FAR_LEFT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (f_lcliff_v < hole_threshold)
    {
        reading = f_lcliff_v;
        hole(reading, LEFT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (f_lcliff_v > tape_threshold)
    {
        reading = f_lcliff_v;
        hole(reading, LEFT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (f_rcliff_v < hole_threshold)
    {
        reading = f_rcliff_v;
        hole(reading, RIGHT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (f_rcliff_v > tape_threshold)
    {
        reading = f_rcliff_v;
        hole(reading, RIGHT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (rcliff_v < hole_threshold)
    {
        reading = rcliff_v;
        hole(reading, FAR_RIGHT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }

    if (rcliff_v > tape_threshold)
    {
        reading = rcliff_v;
        hole(reading, FAR_RIGHT);
        oi_update(sensor_data);
        hazard_detected = 1;
    }
    return hazard_detected;
}

void bump(int dir){
    char data[100] = "";
    if (dir == LEFT)
            {
                sprintf(data, "BUMP LEFT\n\r");
            }
    if (dir == RIGHT)
    {
        sprintf(data, "BUMP RIGHT\n\r");
    }
    uart_sendStr(data);
}

void hole(int reading, int dir)
{
    int hole_threshold = 100;//bot 7
    int tape_threshold = 2000;//bot 7
    //detect hole range for boundary
    if (reading > tape_threshold)
    {
        uart_sendHole(BOUNDARY, dir);
    }
    //detect hole range for pit
    else if (reading < hole_threshold)
    {
        uart_sendHole(HOLE, dir);

    }
}

