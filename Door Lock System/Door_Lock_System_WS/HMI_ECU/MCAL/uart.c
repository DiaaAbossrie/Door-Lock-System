/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Diaa Ahmed
 *
 *******************************************************************************/

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "../LIB/common_macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Function responsible for initializing the UART device. It sets up the frame format
 * including the number of data bits, parity bit type, and number of stop bits.
 * It also enables the UART and sets the baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr)
{
	uint16 ubrr_value = 0;

	/* U2X = 1 for double transmission speed */
	UCSRA = (1<<U2X);

	// Configure UCSRB register
	UCSRB = (1<<RXEN) | (1<<TXEN); // Enable Receiver and Transmitter

	// Configure UCSRC register
	UCSRC = (1<<URSEL); // Set URSEL to write to UCSRC
	UCSRC &= 0xCF; // Clear previous bits related to parity
	UCSRC |= ((Config_Ptr -> parity) << 4) & 0x30; // Set parity type
	UCSRC &= 0xF7; // Clear previous bits related to stop bits
	UCSRC |= ((Config_Ptr -> stop_bit) << 3) & 0x08; // Set stop bits
	UCSRC &= 0xF9; // Clear previous bits related to data bits
	UCSRC |= ((Config_Ptr -> bit_data) << 1) & 0x06; // Set data bits

	// Calculate the UBRR register value
	ubrr_value = (uint16)(((F_CPU / ((Config_Ptr -> baud_rate) * 8UL))) - 1);

	// Set UBRRH and UBRRL registers
	UBRRH = ubrr_value >> 8;
	UBRRL = ubrr_value;
}

/*
 * Description:
 * Function responsible for sending a byte to another UART device.
 * It waits until the UART Data Register Empty (UDRE) flag is set and
 * then puts the data in the UDR register.
 */
void UART_sendByte(const uint8 data)
{
	while(BIT_IS_CLEAR(UCSRA,UDRE)){} // Wait until UDRE flag is set
	UDR = data; // Put the data in UDR
}

/*
 * Description:
 * Function responsible for receiving a byte from another UART device.
 * It waits until the UART Receive Complete (RXC) flag is set and then
 * reads the data from the UDR register.
 */
uint8 UART_recieveByte(void)
{
	while(BIT_IS_CLEAR(UCSRA,RXC)){} // Wait until RXC flag is set
	return UDR; // Read and return the received data from UDR
}

/*
 * Description:
 * Function responsible for sending a string through UART to another UART device.
 * It iterates through the string and calls UART_sendByte() for each character.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
}

/*
 * Description:
 * Function responsible for receiving a string until the '#' symbol through UART
 * from another UART device. It stores the received string in the provided array.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;
	Str[i] = UART_recieveByte(); // Receive the first byte

	while(Str[i] != '#') // Continue receiving until '#' is encountered
	{
		i++;
		Str[i] = UART_recieveByte(); // Receive next byte
	}

	Str[i] = '\0'; // Terminate the string with null character
}
