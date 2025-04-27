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

void move_forward(oi_t *sensor_data, double distance_mm)
{
    double CALIBRATION_VALUE = 1.25;
    distance_mm *= CALIBRATION_VALUE;
    double sum = 0;
    while (sum < distance_mm)
    {
        oi_update(sensor_data);
        oi_setWheels(150, 150);
        while (sum < 10)
        { //runs irobot until the sum data is equal to the specified distance
            oi_update(sensor_data);
            sum += sensor_data->distance; //sends a pointer to distance, stores the value in the temp var sum
            uart_sendData(DISTANCE, 10);
        }
//        if (hazards(sensor_data))
//        {
//            break;
//        }
//        else
//        {
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
//        if (hazards(sensor_data))
//        {
//            break;
//        }
//        else
//        {
        sum += sensor_data->distance;
//        }
    }
    oi_setWheels(0, 0);
}

void turn_right(oi_t *sensor_data, double degrees)
{
    double CALIBRATION_VALUE = 0.7;
    degrees *= CALIBRATION_VALUE;
    double sum = 0;
    oi_setWheels(-150, 150); // sets the speed of the left wheel to move backward and the right wheel to move forward, which turns the robot right
    //DEVNOTE: This movement logs the sum value negatively, so the mathematical operations are inverted from the turn_left method.
    while (sum >= -degrees) //turns the irobot right. sum value decriments until it reaches the specified degree value.
    {
        oi_update(sensor_data);
        sum += sensor_data->angle; //sends a pointer to angle, stores the value in the temp var sum
        uart_sendData(ANGLE, 1);
    }
    oi_setWheels(0, 0);
}
void turn_left(oi_t *sensor_data, double degrees)
{
    degrees *= 0.7;
    double sum = 0;
    oi_setWheels(150, -150); //sets the speed of the right wheel to move forward and the left wheel to move backward, which turns the robot left.
    while (sum < degrees) //turns irobot left. sum value increments until it reaches the specified degree value.
    {
        oi_update(sensor_data);
        sum += sensor_data->angle; //sends a pointer to angle, stores the value in the temp var sum
        uart_sendData(ANGLE, -1);
    }
    oi_setWheels(0, 0); // stops the robot when while loop is finished
}

void movement(oi_t *sensor_data)
{
    while (command_flag != 1)
    {
        if (command_flag == 6)
        {
            oi_update(sensor_data);
            move_forward(sensor_data, 10);
        }
        if (command_flag == 7)
        {
            oi_update(sensor_data);
            move_backward(sensor_data, 10);
        }
        if (command_flag == 8)
        {
            turn_left(sensor_data, 1);
        }
        if (command_flag == 9)
        {
            turn_right(sensor_data, 1);
        }
        if (command_flag == 10)
        {
            oi_setWheels(0, 0);
        }
    }
}

char hazards(oi_t *sensor_data) //once roomba bumps, gets called, will retreat, turn, go forward, turn, and subtract the sum value from the distance_mm value to compensate for distance traveled when doing the turning cycle
{
    /*TODO: The sensor works but always detects for LEFT first sicne it is thwe first if.
     * Possible solution: add an if statement that compares between the read values and chooses
     * the highest values.
     */
    int reading;
    oi_update(sensor_data);
    timer_waitMillis(1000);

    if (sensor_data->bumpLeft)
    {
        oi_update(sensor_data);
//        bump(LEFT);
        return 1;
    }

    else if (sensor_data->bumpRight)
    {
        oi_update(sensor_data);
//        bump(RIGHT);
        return 1;
    }

    else if (sensor_data->cliffLeft)
    {
        oi_update(sensor_data);
        reading = sensor_data->cliffLeftSignal;
        hole(reading, FAR_LEFT);
        lcd_printf("LEFT value: %d", reading);
        return 1;
    }

    else if (sensor_data->cliffFrontLeft)
    {
        oi_update(sensor_data);
        reading = sensor_data->cliffFrontLeftSignal;
        hole(reading, LEFT);
        lcd_printf("FRT LEFT value: %d", reading);
        return 1;
    }

    else if (sensor_data->cliffFrontRight)
    {
        oi_update(sensor_data);
        reading = sensor_data->cliffFrontRightSignal;
        hole(reading, RIGHT);
        lcd_printf("FRT RIGHT value: %d", reading);
        return 1;
    }

    else if (sensor_data->cliffRight)
    {
        oi_update(sensor_data);
        reading = sensor_data->cliffRightSignal;
        hole(reading, FAR_RIGHT);
        lcd_printf("RIGHT value: %d", reading);
        return 1;
    }

    else
    {
        return 0;
    }
}

void hole(int reading, int dir)
{
    double tape_threshold; //TODO: find threshold for tape
    double hole_threshold; //TODO: find threshold for hole
    //detect hole range for boundary
    if (tape_threshold)
    {
        uart_sendHole(BOUNDARY, dir);
    }
    //detect hole range for pit
    else if (hole_threshold)
    {
        uart_sendHole(HOLE, dir);

    }
    //TODO: add end_hole if necessary
}

