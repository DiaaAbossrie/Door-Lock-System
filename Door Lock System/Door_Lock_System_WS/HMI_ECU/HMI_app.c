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


#include <avr/io.h> // Standard AVR I/O Definitions
#include "HAL/lcd.h" // LCD Header File
#include "HAL/keypad.h" // Keypad Header File
#include "MCAL/uart.h" // UART Header File
#include "MCAL/timer1.h" // Timer1 Header File
#include <util/delay.h> // Utility functions for delays

// Define constants for communication protocol
#define IS_PASSWORD_SETTED 'Q'              // Indicates if password is already set
#define SETTED 'W'                          // Indicates password is already set in EEPROM
#define NOT_SETTED 'E'                      // Indicates password is not set in EEPROM yet
#define GET_READY_FOR_PASSWORD 'R'          // Request for entering a password
#define CORRECT_PASSWORD 'T'                // Indicates correct password
#define NOT_CORRECT_PASSWORD 'Y'            // Indicates incorrect password
#define OPEN_DOOR 'U'                       // Command to open the door
#define ERROR_ACTION 'I'                    // Action taken when an error occurs
#define GET_READY_FOR_PASSWORD_ONE 'O'      // Request for entering the first password for setup
#define GET_READY_FOR_PASSWORD_TWO 'P'      // Request for entering the second password for setup
#define IS_MATCHED 'A'                     // Indicates that two entered passwords matched
#define MATCHED 'S'                        // Indicates that two entered passwords matched
#define NOT_MATCHED 'D'                    // Indicates that two entered passwords did not match

#define PASSWORD_SIZE 5 // Define password size

uint8 i_counter; // Variable for loop iterations
uint8 password_buffer[PASSWORD_SIZE]; // Array to store password
volatile uint8 timer1_ticks = 0; // Volatile variable for Timer1 ticks

// Configuration for Timer1
Timer1_ConfigType Timer1_config = { .initial_value = 0, .compare_value = 31250,
		.mode = COMPARE, // Compare mode
		.prescaler = PRESCALER_256 // Prescaler value
};

/*
 * Description:
 * This function is responsible for getting the password from the user through the keypad.
 * It uses the KEYPAD_getPressedKey() function to get the pressed keys and displays asterisks (*)
 * to hide the entered characters.
 * It waits until the user presses the '=' key to finish entering the password.
 */
void getPassword(void);

/*
 * Description:
 * This function is responsible for sending the password through UART.
 * It takes a pointer to the password array as an argument and sends each character
 * of the password using the UART_sendByte() function.
 * It also includes a delay of 15 milliseconds after sending each character.
 */
void sendPassword(uint8 *password);

/*
 * Description:
 * This function is used as a callback for Timer1.
 * It is called whenever Timer1 overflows or a compare match occurs.
 * It increments the volatile variable timer1_ticks.
 */
void timer1TickIncrement(void);

int main(void) {
	uint8 tries = 0; // Number of password entry attempts
	uint8 is_matched_f = 1; // Flag to indicate if passwords match
	uint8 is_password_set_f = 0; // Flag to indicate if password is already set
	uint8 is_password_correct_f = 0; // Flag to indicate if entered password is correct

	// UART Configuration
	UART_ConfigType UART_config = { .bit_data = EIGHT_BITS, .parity = NO_PARITY,
			.stop_bit = ONE_STOP_BIT, .baud_rate = 9600 };

	SREG |= 1 << 7; // Enable global interrupts
	UART_init(&UART_config); // Initialize UART communication
	LCD_init(); // Initialize LCD
	Timer1_setCallBack(timer1TickIncrement); // Set Timer1 callback function for ticks

	LCD_displayStringRowColumn(0,3,"Door  Lock");
	LCD_displayStringRowColumn(1,5, "System");
	_delay_ms(2000);
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"By:");
	LCD_displayStringRowColumn(1,1,"Diaa  Abossrie");
	_delay_ms(2000);

	UART_sendByte(IS_PASSWORD_SETTED); // Send request to check if password is already set

	// Check the response received from the Control_ECU
	if (UART_recieveByte() == SETTED) {
		is_password_set_f = 1; // Set flag indicating password is already set
	}

	while (1) {
		if (is_password_set_f) { // Check if password is already set
			tries = 0; // Reset number of password entry attempts
			LCD_clearScreen(); // Clear the LCD screen
			LCD_displayStringRowColumn(0, 0, "+ : Open Door"); // Display option to open the door
			LCD_displayStringRowColumn(1, 0, "- : Change Pass"); // Display option to change the password

			uint8 key = KEYPAD_getPressedKey(); // Get the pressed key from the keypad
			_delay_ms(100); // Delay for 100 milliseconds (debouncing)

			switch (key) {
			case '+':
				_delay_ms(200); // Wait for 200 milliseconds (debouncing)
				while (tries <= 2 && !is_password_correct_f) {
					is_password_correct_f = 0; // Reset flag for correct password entry
					LCD_clearScreen(); // Clear the LCD screen
					LCD_displayStringRowColumn(0, 0, "plz enter pass: "); // Prompt for password entry
					LCD_moveCursor(1, 0); // Move cursor to the next line
					UART_sendByte(GET_READY_FOR_PASSWORD); // Send request for password entry
					getPassword(); // Get password from user
					sendPassword(password_buffer); // Send password through UART
					_delay_ms(15); // Delay for 15 milliseconds after sending each character

					if (UART_recieveByte() == CORRECT_PASSWORD) { // Check if received password is correct
						is_password_correct_f = 1; // Set flag indicating correct password
						UART_sendByte(OPEN_DOOR); // Send command to open the door
						Timer1_init(&Timer1_config); // Initialize Timer1 for timing operations

						// Display messages to indicate door unlocking process
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 6, "DOOR");
						LCD_displayStringRowColumn(1, 2, "IS UNLOCKING");

						// Wait for Timer1 ticks to reach 15 (15 seconds)
						while (timer1_ticks != 15)
							;
						timer1_ticks = 0; // Reset Timer1 ticks
						Timer1_deinit(); // Deinitialize Timer1

						// Reinitialize Timer1 for holding duration
						Timer1_init(&Timer1_config);
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 6, "DOOR");
						LCD_displayStringRowColumn(1, 3, "IS HOLDING");

						// Wait for Timer1 ticks to reach 3 (3 seconds)
						while (timer1_ticks != 3)
							;
						timer1_ticks = 0; // Reset Timer1 ticks
						Timer1_deinit(); // Deinitialize Timer1

						// Reinitialize Timer1 for locking process
						Timer1_init(&Timer1_config);
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 6, "DOOR");
						LCD_displayStringRowColumn(1, 3, "IS LOCKING");

						// Wait for Timer1 ticks to reach 15 (15 seconds)
						while (timer1_ticks != 15)
							;
						timer1_ticks = 0; // Reset Timer1 ticks
						Timer1_deinit(); // Deinitialize Timer1
					} else {
						tries++; // Increment the number of password entry attempts
					}
				}
				if (is_password_correct_f == 0) { // Check if password was incorrect
					UART_sendByte(ERROR_ACTION); // Send error action command
					LCD_clearScreen(); // Clear the LCD screen
					Timer1_init(&Timer1_config); // Initialize Timer1 for timing operations

					// Display unauthorized access message for 60 seconds
					while (timer1_ticks != 60) {
						LCD_displayStringRowColumn(0, 2, "UNAUTHORIZED");
						LCD_displayStringRowColumn(1, 5, "ACCESS");
						_delay_ms(500);
						LCD_clearScreen();
						_delay_ms(500);
					}

					timer1_ticks = 0; // Reset Timer1 ticks
					Timer1_deinit(); // Deinitialize Timer1
				}

				is_password_correct_f = 0; // Reset flag for correct password entry
				break; // Exit the switch statement

			case '-':
				_delay_ms(200); // Wait for 200 milliseconds (debouncing)
				while (tries <= 2 && !is_password_correct_f) {
					is_password_correct_f = 0; // Reset flag for correct password entry
					LCD_clearScreen(); // Clear the LCD screen
					LCD_displayStringRowColumn(0, 0, "plz enter pass: "); // Prompt for password entry
					LCD_moveCursor(1, 0); // Move cursor to the next line
					UART_sendByte(GET_READY_FOR_PASSWORD); // Send request for password entry
					getPassword(); // Get password from user
					sendPassword(password_buffer); // Send password through UART
					_delay_ms(15); // Delay for 15 milliseconds after sending each character

					if (UART_recieveByte() == CORRECT_PASSWORD) { // Check if received password is correct
						is_password_set_f = 0; // Reset flag for password set
						is_password_correct_f = 1; // Set flag indicating correct password
					} else {
						tries++; // Increment the number of password entry attempts
					}
				}
				if (is_password_correct_f == 0) { // Check if password was incorrect
					UART_sendByte(ERROR_ACTION); // Send error action command
					LCD_clearScreen(); // Clear the LCD screen
					Timer1_init(&Timer1_config); // Initialize Timer1 for timing operations

					// Display unauthorized access message for 60 seconds
					while (timer1_ticks != 60) {
						LCD_displayStringRowColumn(0, 2, "UNAUTHORIZED");
						LCD_displayStringRowColumn(1, 5, "ACCESS");
						_delay_ms(500);
						LCD_clearScreen();
						_delay_ms(500);
					}

					timer1_ticks = 0; // Reset Timer1 ticks
					Timer1_deinit(); // Deinitialize Timer1
				}

				break; // Exit the switch statement
			}
		} else {
			is_matched_f = 1; // Reset flag for password match
			LCD_clearScreen(); // Clear the LCD screen
			LCD_displayString("plz enter pass: "); // Prompt for password entry
			LCD_moveCursor(1, 0); // Move cursor to the next line
			UART_sendByte(GET_READY_FOR_PASSWORD_ONE); // Send request for first password entry
			getPassword(); // Get password from user
			sendPassword(password_buffer); // Send password through UART

			LCD_clearScreen(); // Clear the LCD screen
			LCD_displayString("plz re-enter the"); // Prompt for re-entering password
			LCD_displayStringRowColumn(1, 0, "same pass: "); // Display message for re-entering password
			LCD_moveCursor(1, 11); // Move cursor to the last character position

			UART_sendByte(GET_READY_FOR_PASSWORD_TWO); // Send request for second password entry
			getPassword(); // Get password from user
			sendPassword(password_buffer); // Send password through UART

			_delay_ms(15); // Delay for 15 milliseconds after sending password

			UART_sendByte(IS_MATCHED); // Send request to check if passwords match
			is_matched_f = UART_recieveByte(); // Receive response for password match

			if (is_matched_f == MATCHED) { // Check if passwords matched
				is_password_set_f = 1; // Set flag indicating password is set
				is_matched_f = 0; // Reset flag for password match
			} else {
				LCD_clearScreen(); // Clear the LCD screen
				LCD_displayStringRowColumn(0, 3, "UNMATCHED!"); // Display unmatched message
				LCD_displayStringRowColumn(0, 3, "TRY  AGAIN"); // Display retry message
			}
		}
	}
	return 0;
}

void sendPassword(uint8 *password) {
    for (i_counter = 0; i_counter < PASSWORD_SIZE; i_counter++) {
        UART_sendByte(*(password + i_counter)); // Send each character of the password through UART
        _delay_ms(15); // Delay for 15 milliseconds after sending each character
    }
}

void getPassword(void) {
    for (i_counter = 0; i_counter < PASSWORD_SIZE; i_counter++) {
        *(password_buffer + i_counter) = KEYPAD_getPressedKey(); // Store pressed keys in password_buffer array
        LCD_displayCharacter('*'); // Display asterisk to hide entered characters
        _delay_ms(500); // Delay for 500 milliseconds (debouncing)
    }
    while (KEYPAD_getPressedKey() != '='); // Wait until user presses '=' key (finish entering password)
}

void timer1TickIncrement(void) {
    timer1_ticks++; // Increment the volatile variable timer1_ticks
}

