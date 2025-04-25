/*
 * movement.c
 *
 *  Created on: Feb 6, 2025
 *      Author: mansa
 */

#include "open_interface.h"
#include "movement.h"
#include "gui.h"

double turn_right(oi_t *sensor_data, double degrees)
{
    degrees *= 0.7;
    double sum = 0;
    oi_setWheels(-175, 175); // sets the speed of the left wheel to move backward and the right wheel to move forward, which turns the robot right
    //DEVNOTE: This movement logs the sum value negatively, so the mathematical operations are inverted from the turn_left method.
    while (sum >= -degrees) //turns the irobot right. sum value decriments until it reaches the specified degree value.
    {
        oi_update(sensor_data);
        sum += sensor_data->angle; //sends a pointer to angle, stores the value in the temp var sum
        uart_sendData(ANGLE, -1);
    }
    oi_setWheels(0, 0);
    return sum;
}
double turn_left(oi_t *sensor_data, double degrees)
{
    degrees *= 0.7;
    double sum = 0;
    oi_setWheels(175, -175); //sets the speed of the right wheel to move forward and the left wheel to move backward, which turns the robot left.
    while (sum < degrees) //turns irobot left. sum value increments until it reaches the specified degree value.
    {
        oi_update(sensor_data);
        sum += sensor_data->angle; //sends a pointer to angle, stores the value in the temp var sum
        uart_sendData(ANGLE, 1);
    }
    oi_setWheels(0, 0); // stops the robot when while loop is finished
    return sum;
}
double movement_bumping(oi_t *sensor_data, double distance_mm) //once roomba bumps, gets called, will retreat, turn, go forward, turn, and subtract the sum value from the distance_mm value to compensate for distance traveled when doing the turning cycle
{
    double CALIBRATION_VALUE = 1.25;
    double sum = 0;
    distance_mm *= CALIBRATION_VALUE;
    while (sum < distance_mm)
    {
        oi_update(sensor_data);

        oi_setWheels(300, 300); //sets the speed of the left and right wheels, respectively

        while (sum < 10)
        { //runs irobot until the sum data is equal to the specified distance
            oi_update(sensor_data);
            sum += sensor_data->distance; //sends a pointer to distance, stores the value in the temp var sum
            uart_sendData(DISTANCE, 10);
        }

        if (sensor_data->bumpLeft)
        {
            oi_update(sensor_data);
            bump(LEFT);
            break;
        }

        else if (sensor_data->bumpRight)
        {
            oi_update(sensor_data);
            bump(RIGHT);
            break;
        }

        else if (sensor_data->cliffLeft)
        {
            oi_update(sensor_data);
            hole(RIGHT);
            break;
        }

        else if (sensor_data->cliffRight)
        {
            oi_update(sensor_data);
            hole(RIGHT);
            break;
        }

        else
        {
            sum += sensor_data->distance;
        }

    }
    oi_setWheels(0, 0);

    return 0;
}

void bump(int dir)
{
    uart_sendData(BUMP, dir);
}

#warning add parameter for the strength of the hole, compare to thresholds in if statements

void hole(int dir)
{
    double tape_threshold;//TODO: find threshold for tape
    double hole_threshold;//TODO: find threshold for hole
    //detect hole range for boundary
    if (tape_threshold)
    {
        uart_sendHole(BOUNDARY, dir);
    }
    else if (hole_threshold){
        uart_sendHole(HOLE, dir);
    }
}

