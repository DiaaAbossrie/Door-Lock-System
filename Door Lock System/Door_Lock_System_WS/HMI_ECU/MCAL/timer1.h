/******************************************************************************
 *
 * Module: TIMER1
 *
 * File Name: timer1.h
 *
 * Description: Header file for the AVR TIMER1 driver
 *
 * Author: Diaa Ahmed
 *
 *******************************************************************************/
#ifndef TIMER1_H_
#define TIMER1_H_

#include "../LIB/std_types.h"

// Enumeration for different prescaler values
typedef enum{
	NO_CLK,
	NO_PRESCALER,
	PRESCALER_8,
	PRESCALER_64,
	PRESCALER_256,
	PRESCALER_1024
}Timer1_Prescaler;

// Enumeration for different modes of Timer1
typedef enum{
	NORMAL,
	COMPARE = 4
}Timer1_Mode;

// Structure to hold Timer1 configuration settings
typedef struct{
	uint16 initial_value; // Initial value of the counter
	uint16 compare_value; // Value to compare with in compare mode
	Timer1_Prescaler prescaler; // Prescaler for clock division
	Timer1_Mode mode; // Mode of operation (Normal or Compare)
}Timer1_ConfigType;

// Function prototypes for Timer1 driver

/*
 * Description:
 * Function to initialize Timer1 with the specified configuration settings.
 * It sets the initial value, compare value, prescaler, and mode.
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*
 * Description:
 * Function to deinitialize Timer1.
 * It turns off the timer and disables related interrupts.
 */
void Timer1_deinit(void);

/*
 * Description:
 * Function to set a callback function for Timer1.
 * The callback function will be called when a timer event occurs (Compare Match A or Overflow).
 */
void Timer1_setCallBack(void (*a_ptr)(void));

#endif /* TIMER1_H_ */
