/*-----------------------------------------------------------------------------
 * Module Name: DC Motor Control
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for DC motor control module.
 *
 * Author: Diaa Abossrie
 *-----------------------------------------------------------------------------*/

#include "../MCAL/gpio.h"       // Include GPIO header file for pin operations
#include "../LIB/std_types.h"  // Include standard data types header file for data types
#include "dc_motor.h"   // Include DC motor header file for function prototypes

// Function to initialize the DC motor
void DcMotor_Init(void){

    // Configure PINA and PINB as output pins for motor terminals
    GPIO_setupPinDirection(PORT_ID, PINA_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(PORT_ID, PINB_ID, PIN_OUTPUT);

    // Set initial states of PINA and PINB to LOGIC_LOW for motor stop
    GPIO_writePin(PORT_ID, PINA_ID, LOGIC_LOW);
    GPIO_writePin(PORT_ID, PINB_ID, LOGIC_LOW);
}

// Function to control the DC motor rotation
void DcMotor_Rotate(DcMotor_State state, uint8 speed){

    switch(state){
        case CW:
            // Drive the motor in Clockwise direction
            GPIO_writePin(PORT_ID, PINA_ID, LOGIC_LOW);
            GPIO_writePin(PORT_ID, PINB_ID, LOGIC_HIGH);
            // Start PWM to control motor speed based on speed parameter
            PWM_Timer0_Start(((speed * 255) / 100));
            break;
        case A_CW:
            // Drive the motor in Anti-Clockwise direction
            GPIO_writePin(PORT_ID, PINA_ID, LOGIC_HIGH);
            GPIO_writePin(PORT_ID, PINB_ID, LOGIC_LOW);
            // Start PWM to control motor speed based on speed parameter
            PWM_Timer0_Start(((speed * 255) / 100));
            break;
        case STOP:
            // Stop the motor by setting both terminals to LOGIC_LOW
            GPIO_writePin(PORT_ID, PINA_ID, LOGIC_LOW);
            GPIO_writePin(PORT_ID, PINB_ID, LOGIC_LOW);
            // Start PWM to control motor speed based on speed parameter
            PWM_Timer0_Start(((speed * 255) / 100));
            break;
    }
}
