/******************************************************************************
 *
 * Module: HMI Application
 *
 * File Name: HMI_app.c
 *
 * Description: Source file for the Human Machine Interface (HMI) application
 *              of the Door Locker Security System.
 *
 * Author: Diaa Ahmed
 *
 *******************************************************************************/

#include "timer1.h"
#include "avr/io.h"
#include "avr/interrupt.h"

/* Define a pointer to function that will hold the address of the callback function */
void (*Timer1_CallBack_Ptr)(void) = NULL_PTR;

/*
 * Description:
 * Initialize TIMER1 with the specified configurations.
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr){
    /* Set initial value for Timer1 counter */
    TCNT1 = Config_Ptr -> initial_value;

    /* Set compare value for Timer1 */
    OCR1A = Config_Ptr -> compare_value;

    /* Configure Timer1 mode (WGM10, WGM11 bits) */
    TCCR1A &= 0xFC;
    TCCR1A |= (Config_Ptr -> mode) & 0x04;

    /* Configure Timer1 mode (WGM12, WGM13 bits) and Prescaler (CS10, CS11, CS12 bits) */
    TCCR1B &= 0xE7;
    TCCR1B |= ((Config_Ptr -> mode) << 1) & 0x18;
    TCCR1B &= 0xF8;
    TCCR1B |= (Config_Ptr -> prescaler) & 0x07;

    /* Enable the Timer1 compare interrupt and overflow interrupt */
    TIMSK |= (1<<OCIE1A) | (1<<TOIE1);
}

/*
 * Description:
 * Deinitialize TIMER1 by setting its control registers to zero.
 */
void Timer1_deinit(void){
    TCCR1B = 0x00;
    TIMSK &= 0xEB;
}

/*
 * Description:
 * Set the callback function for TIMER1.
 */
void Timer1_setCallBack(void (*a_ptr)(void)){
    Timer1_CallBack_Ptr = a_ptr;
}

/* Interrupt Service Routine for Timer1 Compare Match A */
ISR(TIMER1_COMPA_vect){
    (*Timer1_CallBack_Ptr)();
}

/* Interrupt Service Routine for Timer1 Overflow */
ISR(TIMER1_OVF_vect){
    (*Timer1_CallBack_Ptr)();
}
