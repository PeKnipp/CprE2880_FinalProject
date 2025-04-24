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

void uart_sendData(int dataType, int dataVal)
{
    char data[100];
    if (dataType == DISTANCE)
    {
        sprintf(data, "Distance %d\n", dataVal);
        uart_sendStr(data);
    }
    if (dataType == ANGLE)
    {
        sprintf(data, "Angle %d\n", dataVal);
        uart_sendStr(data);
    }
    if (dataType == BUMP)
    {
        if (dataType == FAR_LEFT)
        {
            sprintf(data, "BUMP FAR_LEFT\n");
        }
        if (dataVal == LEFT)
        {
            sprintf(data, "BUMP LEFT\n");
        }
        if (dataVal == RIGHT)
        {
            sprintf(data, "BUMP RIGHT\n");
        }
        if (dataVal == FAR_RIGHT)
        {
            sprintf(data, "BUMP FAR_RIGHT\n");
        }
        uart_sendStr(data);
    }
}

void uart_sendHole(int holeType, int holeDir)
{
    char data[100];
    //process which sensor detected the hole
//    if (holeDir == 0)
//    {
//        char direction[] == "LEFT";
//    }
//    if (holeDir == 1)
//    {
//        char direction[] == "RIGHT";
//    }
//    if (holeDir == 2)
//    {
//        char direction[] == "FAR_LEFT";
//    }
//    if (holeDir == 3)
//    {
//        char direction[] == "FAR_RIGHT";
//    }
//    else()
//        {
//        char direction[] == "OUT OF BOUNDS";
//    }

//process the hole type and respond accordingly
    if (holeType == HOLE)
    {
        if (holeDir == FAR_LEFT)
        {
            sprintf(data, "HOLE HOLE FAR_LEFT");
        }
        if (holeDir == LEFT)
        {
            sprintf(data, "HOLE HOLE LEFT");
        }
        if (holeDir == RIGHT)
        {
            sprintf(data, "HOLE HOLE RIGHT");
        }
        if (holeDir == FAR_RIGHT)
        {
            sprintf(data, "HOLE HOLE FAR_RIGHT");
        }
    }

    if (holeType == BOUNDARY)
    {
        if (holeDir == FAR_LEFT)
        {
            sprintf(data, "HOLE BOUNDARY FAR_LEFT");
        }
        if (holeDir == LEFT)
        {
            sprintf(data, "HOLE BOUNDARY LEFT");
        }
        if (holeDir == RIGHT)
        {
            sprintf(data, "HOLE BOUNDARY RIGHT");
        }
        if (holeDir == FAR_RIGHT)
        {
            sprintf(data, "HOLE BOUNDARY FAR_RIGHT");
        }

    }

    if (holeType == END)
    {
        if (holeDir == FAR_LEFT)
        {
            sprintf(data, "HOLE END FAR_LEFT");
        }
        if (holeDir == LEFT)
        {
            sprintf(data, "HOLE END LEFT");
        }
        if (holeDir == RIGHT)
        {
            sprintf(data, "HOLE END RIGHT");
        }
        if (holeDir == FAR_RIGHT)
        {
            sprintf(data, "HOLE END FAR_RIGHT");
        }
    }
}
