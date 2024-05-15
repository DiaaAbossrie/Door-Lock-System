/*-----------------------------------------------------------------------------
 * Module Name: PWM (Pulse Width Modulation) Control
 *
 * File Name: timer0_pwm.c
 *
 * Description: Source file for PWM module. Contains function to start PWM
 *              using Timer0.
 *
 * Author: Diaa Abossrie
 *-----------------------------------------------------------------------------*/

#include <avr/io.h>      // Include AVR IO header file for register access
#include "../LIB/std_types.h"   // Include standard data types header file
#include "gpio.h"        // Include GPIO header file for pin operations
#include "timer0_pwm.h"         // Include PWM header file for function prototypes

void PWM_Timer0_Start(uint8 duty_cycle){

    TCNT0 = 0;  // Initialize Timer0 counter value

    OCR0  = duty_cycle;  // Set duty cycle using Output Compare Register

    GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT); // Configure pin as output

    // Configure Timer0 for Fast PWM mode, non-inverting mode, and set prescaler
    TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}
