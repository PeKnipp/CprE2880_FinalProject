/**
 * main.c
 *
 * The main file for our project implementation
 *
 * @author Christopher Ledo, Peter Knipper
 *
 */

#include "Timer.h"
#include "lcd.h"
#include "ping.h"  // For scan sensors
#include "servo.h"
#include "uart-interrupt.h"
#include "open_interface.h"
#include "movement.h"
#include "adc.h" //for IR sensors
#include "scan.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "button.h"

/*
 * Fixed errors related to code that wasnt written for CCS.
 * I assume thats supposed to be our struct with values from the scan.
 * Would like to know from what file the code is from and sit down with Peter to talk about their code.
 * No clue why struct deoesnt work
 */

//imported the functions from lab 8 to lab 10 into main
typedef struct
{
    char firstEdge;
    char secondEdge;
    float distance;
    float objectAngle;
    char radialWidth;
    float linearWidth;
}fieldObject;

void main(){

	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	button_init();
	uart_interrupt_init();
	adc_init();
	ping_init();
	servo_init();

	oi_t *sensor_data = oi_alloc();
	oi_init (sensor_data);


    char i;
    char j = 0;  //number of legit objects starts at zero
    char manual = 0;
    char wheels_on = 0;
    char info[100];
    char angleInc = 2;
//    const int scanArraySizes = 91; //180/angleInc + 1;
    float PINGdata[91]; //initialize arrays to only have as many entrys as scans
    short IRdata[91]; //changed from short
    const int SONAR = 0;
    const int IR = 1;
    fieldObject objects[6] = {0};
    scan(0,SONAR);
    timer_waitMillis(200);

    /*--------------------------------------CALIBRATION--------------------------------------*/

    /*----------SERVO----------*/
    //shows the left and right calibration values
    //TODO for each CyBot: assign true_0 and true_180 values in servo.c
//    servo_calibrate();

    /*----------ADC----------*/
    //TODO for each CyBot: assign coeff and power values in scan.c
//    scan(90,SONAR);
//    adc_calibrate();

    /*---------------------------------------------------------------------------------------*/


    uart_sendStr("\n\rReady for scan!\n\r\n\r");
	while(command_flag != 3) //'q' character quits the program when not scanning
	{

	    if (command_flag == 1){ //'g' starts the scan
	        for (i = 0; i < 91; i++){
	            PINGdata[i] = 0;
	            IRdata[i] = 0;
	        }



	        sprintf(info, "\n\rANGLE:       PING:       IR:     \n\r");
	        uart_sendStr(info);

	        for (i=0; i<=180/angleInc; i++){

                if (command_flag == 2){ //'s' prematurely ends the scan
                    sprintf(info, "Scan cancelled\n\r");
                    uart_sendStr(info);
                    break;
                }

                if (command_flag == 5){ //'m' switches mode
                    command_flag = 0;
                    if (!manual){
                        sprintf(info, "Switching to manual scan\n\r");
                        uart_sendStr(info);
                    }
                    else{
                        sprintf(info, "Switching to auto scan\n\r");
                        uart_sendStr(info);
                    }
                    manual = !manual;

                }

                timer_waitMillis(20); //wait for servo
                PINGdata[i] = scan(i*angleInc, SONAR);
                IRdata[i] = scan(i*angleInc, IR);
	            sprintf(info, "%d       %f      %d      \n\r", i * angleInc, PINGdata[i], IRdata[i]);
	            uart_sendStr(info);
	        }

	        j = 0;
	        for (i = 0; i < 91; i+=angleInc){   //sizeof(IRdata) = 180/angleInc + 1
	            command_flag = 0;
	            if (command_flag == 5){ //'m' switches mode
                    if (!manual){
                        sprintf(info, "Switching to manual scan\n\r");
                        uart_sendStr(info);
                    }
                    else{
                        sprintf(info, "Switching to auto scan\n\r");
                        uart_sendStr(info);
                    }
                    manual = !manual;

                }


	            if (IRdata[i] >= 750){ //if IR reports a number greater than 1000, probably an object

	                objects[j].firstEdge = i * angleInc;    //set the first edge of the object at the inital instance

	                do{

	                    objects[j].secondEdge = i * angleInc;   //while still reporting >1000 values incrementally set the second edge of the object
	                    i++;

	                }while (i < 91 && IRdata[i] >= 750);

	                objects[j].radialWidth = objects[j].secondEdge - objects[j].firstEdge;  //set the width

	                if (objects[j].radialWidth > angleInc){ //only count instances that have a radial width larger than the increment
	                    j++;
	                }

	            }
	        }

	        int smallestObject = 0;
	        for (i = 0; i<j; i++){
	            command_flag = 0;
	            if (command_flag == 5){ //'m' switches mode
                    if (!manual){
                        sprintf(info, "Switching to manual scan\n\r");
                        uart_sendStr(info);
                    }
                    else{
                        sprintf(info, "Switching to auto scan\n\r");
                        uart_sendStr(info);
                    }
                    manual = !manual;

                }

                objects[i].objectAngle = (objects[i].firstEdge + objects[i].secondEdge)/2.0;
                objects[i].distance = PINGdata[(int)(objects[i].objectAngle / angleInc)];
                objects[i].linearWidth = 2.0*objects[i].distance*tan((objects[i].radialWidth*(M_PI/180))/2.0);
                if (objects[i].linearWidth < objects[smallestObject].linearWidth){
                    smallestObject = i;
                }
	            sprintf(info, "\n\rObject_Number: %i\n\r", i+1);
                uart_sendStr(info);
                sprintf(info, "Object_Angle: %f\n\r", objects[i].objectAngle);
                uart_sendStr(info);
                sprintf(info, "Object_Distance: %f\n\r", objects[i].distance);
                uart_sendStr(info);
                sprintf(info, "Object_Width: %f\n\r", objects[i].linearWidth);
                uart_sendStr(info);
                sprintf(info, "RW: %i\n\r", objects[i].radialWidth);
                uart_sendStr(info);
            }

	        sprintf(info,"Smallest object: %d", smallestObject + 1);
	        uart_sendStr(info);

	        if(!manual){
                if(objects[smallestObject].objectAngle > 90){
                    sprintf(info,"\n\rTurning Left\n\r");
                    uart_sendStr(info);
                    servo_move(0);
                    turn_left(sensor_data, (objects[smallestObject].objectAngle - 90.0 - 5.0));   //enters inf loop at open_interface.c -> char oi_uartReceive(void) -> while ((UART4_FR_R & UART_FR_RXFE))
                }
                else if(objects[smallestObject].objectAngle < 90){
                    sprintf(info,"\n\rTurning Right\n\r");
                    uart_sendStr(info);
                    servo_move(0);
                    turn_right(sensor_data, (90.0 - objects[smallestObject].objectAngle - 5.0));  //enters inf loop at open_interface.c -> char oi_uartReceive(void) -> while ((UART4_FR_R & UART_FR_RXFE))
                }
                else {
                    sprintf(info,"\n\rNot Turning!\n\r");
                    uart_sendStr(info);
                    servo_move(0);
                }

                sprintf(info,"Moving!\n\r");
                uart_sendStr(info);
                move_forward(sensor_data, objects[smallestObject].distance * 10 - 100);
	        }

            if (command_flag == 4 && !manual){
                command_flag = 1;
                sprintf(info, "RESCANNING\n\r");
                uart_sendStr(info);
            }
            else if(manual){
                wheels_on = 0;
                while (command_flag != 1){
                    if (command_flag == 6){
                        if (!wheels_on){
                            oi_setWheels(200, 200);
                            wheels_on = !wheels_on;

                        }
                    }
                    if (command_flag == 7){
                        if (!wheels_on){
                            oi_setWheels(-200, -200);
                            wheels_on = !wheels_on;

                        }
                    }
                    if (command_flag == 8){
                        if (!wheels_on){
                                oi_setWheels(150, -150);
                                wheels_on = !wheels_on;

                            }
                    }
                    if (command_flag == 9){
                        if (!wheels_on){
                            oi_setWheels(-150, 150);
                            wheels_on = !wheels_on;

                        }
                    }
                    if (command_flag == 10){
                        if (wheels_on){
                            oi_setWheels(0, 0);
                            wheels_on = !wheels_on;
                        }
                    }
                }
            }
            else command_flag = 0;
	    }
	}
    oi_free(sensor_data);
    return;
}

