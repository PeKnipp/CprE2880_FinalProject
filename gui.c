/**
 * gui.c functions for communication with our Python Graphical User Interface
 *
 *  @author Christopher Ledo
 *  @date 4/24/2025
 */

#include <stdio.h>
#include <uart.h>
#include <gui.h>

/**
 * A replacement for the cyBOT_Scan function from previous labs
 */

volatile const int DISTANCE = 0;
volatile const int ANGLE = 1;
volatile const int BUMP = 2;

volatile const int HOLE = 0;
volatile const int BOUNDARY = 1;
volatile const int END = 2;

volatile const int FAR_LEFT = 0;
volatile const int LEFT = 1;
volatile const int RIGHT = 2;
volatile const int FAR_RIGHT = 3;

void uart_sendData(int dataType, double dataVal){
    char data[100];
        if (dataType == DISTANCE)
        {
            sprintf(data, "DISTANCE %f\n\r", dataVal);
            uart_sendStr(data);
        }
        if (dataType == ANGLE)
        {
            sprintf(data, "ANGLE %f\n\r", dataVal);
            uart_sendStr(data);
        }
}

void uart_sendBump(int dataType, int dataVal)
{
    char data[100];
    if (dataType == BUMP)
    {
        if (dataType == FAR_LEFT)
        {
            sprintf(data, "BUMP FAR_LEFT\n\r");
        }
        if (dataVal == LEFT)
        {
            sprintf(data, "BUMP LEFT\n\r");
        }
        if (dataVal == RIGHT)
        {
            sprintf(data, "BUMP RIGHT\n\r");
        }
        if (dataVal == FAR_RIGHT)
        {
            sprintf(data, "BUMP FAR_RIGHT\n\r");
        }
        uart_sendStr(data);
    }
}

void uart_sendHole(int holeType, int holeDir)
{
    char data[100];
//process the hole type and respond accordingly
    if (holeType == HOLE)
    {
        if (holeDir == FAR_LEFT)
        {
            sprintf(data, "HOLE HOLE FAR_LEFT\n\r");
        }
        if (holeDir == LEFT)
        {
            sprintf(data, "HOLE HOLE LEFT\n\r");
        }
        if (holeDir == RIGHT)
        {
            sprintf(data, "HOLE HOLE RIGHT\n\r");
        }
        if (holeDir == FAR_RIGHT)
        {
            sprintf(data, "HOLE HOLE FAR_RIGHT\n\r");
        }
    }

    if (holeType == BOUNDARY)
    {
        if (holeDir == FAR_LEFT)
        {
            sprintf(data, "HOLE BOUNDARY FAR_LEFT\n\r");
        }
        if (holeDir == LEFT)
        {
            sprintf(data, "HOLE BOUNDARY LEFT\n\r");
        }
        if (holeDir == RIGHT)
        {
            sprintf(data, "HOLE BOUNDARY RIGHT\n\r");
        }
        if (holeDir == FAR_RIGHT)
        {
            sprintf(data, "HOLE BOUNDARY FAR_RIGHT\n\r");
        }

    }

    if (holeType == END)
    {
        if (holeDir == FAR_LEFT)
        {
            sprintf(data, "HOLE END FAR_LEFT\n\r");
        }
        if (holeDir == LEFT)
        {
            sprintf(data, "HOLE END LEFT\n\r");
        }
        if (holeDir == RIGHT)
        {
            sprintf(data, "HOLE END RIGHT\n\r");
        }
        if (holeDir == FAR_RIGHT)
        {
            sprintf(data, "HOLE END FAR_RIGHT\n\r");
        }
    }
    uart_sendStr(data);
}

extern void uart_sendObject(double diameter, double angle, double distance){
    char data[100];
    sprintf(data, "OBJECT %f %f %f\n\r", distance, angle, diameter);
    uart_sendStr(data);
}
