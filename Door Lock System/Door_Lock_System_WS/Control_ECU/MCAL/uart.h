/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Diaa Ahmed
 *
 *******************************************************************************/
#ifndef UART_H_
#define UART_H_

#include "../LIB/std_types.h"

// Enumeration for different configurations of UART data bits
typedef enum{
	FIVE_BITS,
	SIX_BITS,
	SEVEN_BITS,
	EIGHT_BITS,
	NINE_BITS = 7
}UART_BitData;

// Enumeration for different configurations of UART parity bit
typedef enum{
	NO_PARITY,
	EVEN_PARITY = 2,
	ODD_PARITY
}UART_Parity;

// Enumeration for different configurations of UART stop bits
typedef enum{
	ONE_STOP_BIT,
	TWO_STOP_BITS
}UART_StopBit;

// Structure to hold UART configuration settings
typedef struct{
	UART_BitData bit_data; // Number of data bits
	UART_Parity parity; // Parity type
	UART_StopBit stop_bit; // Number of stop bits
	uint32 baud_rate; // Baud rate for communication
}UART_ConfigType;

// Function prototypes for UART driver

/*
 * Description:
 * Function to initialize the UART device with the specified configuration settings.
 * It sets the data bits, parity, stop bits, and baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description:
 * Function to send a byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description:
 * Function to receive a byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description:
 * Function to send a string through UART to another UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description:
 * Function to receive a string until the '#' symbol through UART from another UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
