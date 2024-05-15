/*-----------------------------------------------------------------------------
 * Module Name: PWM (Pulse Width Modulation) Control
 *
 * File Name: timer0_pwm.h
 *
 * Description: Header file for PWM module. Contains function prototypes and
 *              definitions related to PWM operations using Timer0.
 *
 * Author: Diaa Abossrie
 *-----------------------------------------------------------------------------*/

#ifndef TIMER0_PWM_H_
#define TIMER0_PWM_H_

#include "../LIB/std_types.h"  // Include standard data types header file

// Function to start PWM using Timer0
void PWM_Timer0_Start(uint8 duty_cycle);

#endif
