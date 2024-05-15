/*-----------------------------------------------------------------------------
 * Module Name: DC Motor Control
 *
 * File Name: dc_motor.h
 *
 * Description: Header file for DC motor control module.
 *
 * Author: Diaa Abossrie
 *-----------------------------------------------------------------------------*/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "../LIB/std_types.h" // Include standard data types header file for data types
#include "../MCAL/timer0_pwm.h"      // Include PWM header file for motor speed control

#define PORT_ID PORTB_ID // Define PORT_ID as PORTB_ID for motor control

#define PINA_ID PIN1_ID  // Define PINA_ID as PIN0_ID for one motor terminal
#define PINB_ID PIN2_ID  // Define PINB_ID as PIN1_ID for the other motor terminal

#define FULL_SPEED 100
#define ZERO_SPEED 0

// Define an enumeration type DcMotor_State to represent motor states
typedef enum{
    CW,    // Clockwise rotation
    A_CW,  // Anti-clockwise rotation
    STOP   // Motor stop
}DcMotor_State;

// Function to initialize the DC motor
void DcMotor_Init(void);

// Function to control the DC motor rotation
void DcMotor_Rotate(DcMotor_State state, uint8 speed);

#endif
