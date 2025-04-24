/*
 * adc.h
 *
 *  Created on: Mar 25, 2025
 *      Author: pknipper
 */

#ifndef LAB6_CODE_FILES_ADC_H_
#define LAB6_CODE_FILES_ADC_H_

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <stdbool.h>

void adc_init();
uint16_t adc_read();
void adc_calibrate();

#endif /* LAB6_CODE_FILES_ADC_H_ */
