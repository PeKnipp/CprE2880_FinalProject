/*
 * objects.c
 *
 *  Created on: Feb 11, 2025
 *      Authors: Christopher Ledo and Manuel Santana
 */

#include "open_interface.h"
#include "servo.h"
#include "scan.h"
#include "movement.h"
#include "uart.h"
#include "driverlib/interrupt.h"
#include <math.h>
#include "gui.h"
#include <objects.h>

extern volatile int command_flag; // flag to tell the main program a special command was received
volatile double distance_threshold = 0;
volatile int degree_increment = 0;

typedef struct
{
    double angle;
    double diameter;
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
        command_flag = 0;
        command_byte = '\0';
        sprintf(message,
                "Press T to change threshold (current: %.0f)\n\rPress D to change degree increment (current: %i)\n\rPress Q to quit.\n\r",
                distance_threshold, degree_increment);
        uart_sendStr(message);
        while (command_byte != 't' && command_byte != 'd' && command_byte != 'q')
        {

        }
        if (command_byte == 't')
        {
            uart_sendStr("hreshold\n\r");
            command_byte = '\0';
            command_flag = 0;
            sprintf(message, "Enter new threshold: 1-9 hundred mV\n\r");
            uart_sendStr(message);
            while (command_byte < '1' || command_byte > '9')
            {
            }
            if (command_byte >= '1' && command_byte <= '9')
            {

                uart_sendStr("00 mV\n\r");
                distance_threshold = (command_byte - '0') * 100.0;
                command_flag = 0;
                command_byte = '\0';
                sprintf(message, "New voltage threshold is %.0f.\n\r",
                        distance_threshold);
                uart_sendStr(message);
                uart_sendStr(finished);
                return;
            }
        }
        if (command_byte == 'd')
        {
            uart_sendStr("egree increment\n\r");
            command_byte = '\0';
            command_flag = 0;
            sprintf(message, "Enter new degree increment: 1-9 degrees\n\r");
            uart_sendStr(message);
            while (command_byte < '1' || command_byte > '9')
            {
            }
            if (command_byte >= '1' && command_byte <= '9')
            {
                uart_sendStr(" degree(s) at a time.\n\r");
                degree_increment = (command_byte - '0');
                command_byte = '\0';
                command_flag = 0;
                sprintf(message, "New degree increment is %d.\n\r",
                        degree_increment);
                uart_sendStr(message);
                uart_sendStr(finished);
                return;
            }
        }
        if (command_byte == 'q')
        {
            command_flag = 0;
            command_byte = '\0';
            uart_sendStr("uit\n\r");
            return;
        }
    }
}

void print_objects()
{
    if (command_flag == 13)
    {
        uart_sendStr("rint\n\r"); //makes a new line in PuTTY for formatting
        command_flag = 0;
        char object_num[100];
        char angle_info[100];
        char diameter_info[100];
        char distance_info[100];
        int i;
        for (i = 0; i < numObjects; ++i)
        {
            sprintf(object_num, "\n\rObject #: %d\t\n\r", i + 1);
            uart_sendStr(object_num);
            sprintf(angle_info, "Angle to center = %f \t\n\r", objects[i].angle);
            sprintf(diameter_info, "Diameter = %f \t\n\r", objects[i].diameter);
            sprintf(distance_info, "The Distance = %f\t\n\r", objects[i].distance);
            uart_sendStr(angle_info);
            uart_sendStr(diameter_info);
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
//        uart_sendStr("o\n\r"); //makes a new line in PuTTY for formatting
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

        int check1 = 0;
        int check2 = 0;
        int check3 = 0;

        scan(0, SONAR);

        sprintf(heading, "Taking scan\n");
//    sprintf(heading, "Degrees\tIR Voltage (mV)\n\r");

        uart_sendStr(heading);

        for (i = 0; i <= 180; i += degree_increment) //change i+= for the step size of degrees
        {
            if (command_flag == 3)
            {    //stops the scan if stop is updated
                uart_sendStr("uit\n\r"); //makes a new line in PuTTY for formatting
                break;
            }
            currentPing = scan(i, SONAR);
            currentIR = scan(i, IR);

            current = currentIR;
//        sprintf(message, "%d\t\t%i\n\r", i, (int) currentIR);

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
//                    sprintf(gotcha, "Got an object at %d degrees\n\r", i);
//                   uart_sendStr(gotcha);

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
                    objects[object_num].angle = (double)start_angle + ((double)(end_angle - start_angle) / 2.0);
                    objects[object_num].diameter = 2 * M_PI * distance * ((end_angle - start_angle) / 360.00);
                    objects[object_num].distance = distance;
                    uart_sendObject(objects[object_num].diameter, objects[object_num].angle, objects[object_num].distance);
//                    sprintf(detected_object, "Detected an object, created object %d\n\r", object_num + 1);
                    start_angle = -1;
                    end_angle = -1;
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
