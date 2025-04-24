/*
 * communication.c
 *
 *  Created on: Feb 11, 2025
 *      Author: mansa
 */

#include "open_interface.h"
#include "servo.h"
#include "scan.h"
#include "movement.h"
#include "uart.h"
#include "driverlib/interrupt.h"
#include <math.h>
#include <objects.h>

extern volatile int command_flag; // flag to tell the main program a special command was received
volatile double distance_threshold = 0;

typedef struct
{
    int angle_origin;
    double center;
    double arc_length;
    double distance;

} Object;

//global variables
double data;
int angle;
char message[100]; //if value changed to 50 machine will tell u to die, literally
int numObjects;
Object objects[5]; //number of objects expected
char finished[40] = "\n\rSuccessfully ran program\n\n\r";

void change_parameters()
{
    if (command_byte == 'c')
    {
        uart_sendStr("hange\n\r");
        command_byte = '\0';
        sprintf(message,
                "Press T to change threshold (current: %.0f) or press q to quit.\n\r",
                distance_threshold);
        uart_sendStr(message);
        while (command_byte != 'q')
        {
            while (command_byte != 't')
            {

            }
            if (command_byte == 't')
            {
                command_byte = '\0';
                uart_sendStr("hreshold\n\r");
                sprintf(message, "Enter new threshold: 1-9 hundred mV\n\r");
                uart_sendStr(message);
                while (command_byte < '1' || command_byte > '9')
                {
                }
                if (command_byte >= '1' && command_byte <= '9')
                {

                    uart_sendStr("00 mV\n\r");
                    distance_threshold = (command_byte - '0')*100.0;
                    command_byte = '\0';
                    sprintf(message, "New voltage threshold is %.0f.\n\r",
                            distance_threshold);
                    uart_sendStr(message);
                    uart_sendStr(finished);
                    return;
                }
            }
        }
    }
}

void smallest_object()
{
    if (command_flag == 12)
    {
        uart_sendStr("bject\n\r"); //makes a new line in PuTTY for formatting
        command_flag = 0;
        char smallest[40] = "";
        int i;
//        int smallestWidth;
        int smallestIndex = 0;
//        int angle;
//        if (numObjects <= 1)
//        {
//            return; // No need to process if there's only one or no object
//        }
        for (i = 1; i < numObjects; ++i)
        {

            if (objects[i].arc_length < objects[i - 1].arc_length)
            {
                smallestIndex = i; // Update the index of the smallest object
            }
        }
        scan(0, SONAR);
        angle = (objects[smallestIndex].angle_origin
                + objects[smallestIndex].center); // Get the angle of the object with smallest width
        scan(angle, SONAR); // Position the sensor to that angle
        sprintf(smallest, "The smallest object was object %d\n\r",
                smallestIndex + 1);
        uart_sendStr(smallest);
        uart_sendStr(finished);
    }
}

void print_objects()
{
    if (command_flag == 13)
    {
        uart_sendStr("rint\n\r"); //makes a new line in PuTTY for formatting
        command_flag = 0;
        char object_num[100];
        char origin_info[100];
        char width_info[100];
        char center_info[100];
        char distance_info[100];
        int i;
        for (i = 0; i < numObjects; ++i)
        {
            sprintf(object_num, "\n\rObject #: %d\t\n\r", i + 1);
            uart_sendStr(object_num);
//         cyBot_sendByte(objects[i].angle_origin);
            sprintf(origin_info, "The Origin Angle = %d \t\n\r",
                    objects[i].angle_origin);
            sprintf(width_info, "The Arc Length = %lf \t\n\r",
                    objects[i].arc_length);
            sprintf(center_info, "The Center = %f\t\n\r", objects[i].center);
            sprintf(distance_info, "The Distance = %f\t\n\r",
                    objects[i].distance);
            uart_sendStr(origin_info);
            uart_sendStr(width_info);
            uart_sendStr(center_info);
            uart_sendStr(distance_info);
        }
        uart_sendStr(finished); //shows that that program finished and creates whitespace
    }
}

void detect_objects(oi_t *sensor_data, char scanType, double distance_threshold) //CHANGES MADE: Placed the assigning smallest object stuff inside the if and else statements for the different scanTypes and flipped the less than sign for teh IR as the closer the object is to the sensor teh greater the value the sensor will send
{

    if (command_flag == 1)
    {
        command_flag = 0;
        uart_sendStr("o\n\r"); //makes a new line in PuTTY for formatting
        int i = 0;
        double current;
        double currentPing;
        double currentIR;
        int object_num = -1;
        numObjects = 0;
        int start_angle = -1;
        int end_angle = -1;
        double distance;
        int angularWidth = 0;
        char heading[40];
        char gotcha[40];
        char detected_object[40];

        int check1 = 0;
        int check2 = 0;
        int check3 = 0;

        scan(0, SONAR);

//        sprintf(heading, "Taking scan\n\r");
        sprintf(heading, "Degrees\tIR Voltage (mV)\n\r");

        uart_sendStr(heading);

        for (i = 0; i <= 180; i += 1) //change i+= for the step size of degrees
        {
            if (command_byte == 's')
            {    //stops the scan if stop is updated
                uart_sendStr("top\n\r"); //makes a new line in PuTTY for formatting
                break;
            }
            timer_waitMillis(20); //wait for servo
            currentPing = scan(i, SONAR);
            currentIR = scan(i, IR);

            current = currentIR;
            sprintf(message, "%d\t\t%i\n\r", i, (int) currentIR);

            if (distance_threshold < current)
            {
                if (check1 == 0)
                {
                    check1 = 1;
                }
                else if (check2 == 0)
                {
                    check2 = 1;
                }
                else if (check3 == 0)
                {
                    check3 = 1;
                }
                else
                {
                    if (start_angle == -1)
                    {
                        start_angle = i; //changed from i to i-3 to account for filtering
                        distance = currentPing;
//                        distance = measurement[i];
                    }
                    end_angle = i;
                    //print statement
                    sprintf(gotcha, "Got an object at %d degrees\n\r", i);
                    uart_sendStr(gotcha);

                    //start angle
                    angularWidth++;
                }
            }
            else
            {

                if (start_angle != -1 && check1 == 1 && check2 == 1
                        && check3 == 1)
                {
                    object_num += 1;
                    numObjects++;
                    objects[object_num].angle_origin = start_angle;
                    objects[object_num].arc_length = 2 * M_PI * distance
                            * ((end_angle - start_angle) / 360.00);
                    objects[object_num].center = (end_angle - start_angle)
                            / 2.0;
                    objects[object_num].distance = distance;
                    sprintf(detected_object,
                            "Detected an object, created object %d\n\r",
                            object_num + 1);
                    start_angle = -1;
                    end_angle = -1;
                    uart_sendStr(detected_object);
                }
                check1 = 0;
                check2 = 0;
                check3 = 0;
            }

            uart_sendStr(message);
        }
        scan(0, SONAR);
        uart_sendStr(finished); //shows that that program finished and creates whitespace
    }
}

void drive_smallest_object()
{
    if (command_flag == 14)
    {
        int smallestIndex = 0;
        uart_sendStr("Robot driving to smallest o"); //formatting
        oi_t *sensor_data = oi_alloc(); //Always initalize
        oi_init(sensor_data); //Always initalize
        command_flag = 12;
        smallest_object();
        if (angle < 90)
        {
            turn_right(sensor_data, 90 - angle); //accounting for error from 90
            scan(90, SONAR);
            timer_waitMillis(10);
            movement_bumping(sensor_data, objects[smallestIndex].distance);
        }

        else
        {
            turn_left(sensor_data, angle - 90); //accounting for error from 90

            scan(90, SONAR);
            timer_waitMillis(10);
            movement_bumping(sensor_data, objects[smallestIndex].distance);
        }

        command_flag = 0;
        movement_bumping(sensor_data, objects[smallestIndex].distance - 5);
//        }
        command_flag = 0;
        uart_sendStr(finished);
        oi_free(sensor_data);
    }
}
