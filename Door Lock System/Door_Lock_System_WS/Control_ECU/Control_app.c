#include <avr/io.h>
#include "HAL/dc_motor.h"
#include "HAL/buzzer.h"
#include "MCAL/uart.h"
#include "MCAL/timer1.h"
#include <util/delay.h>
#include "MCAL/twi.h"

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

#define IS_PASSWORD_SET_FLAG_LOCATION 0xDD

#define PASSWORD_SIZE 5

uint8 i_counter; // Variable for loop iterations
volatile uint8 timer1_ticks = 0; // Volatile variable for Timer1 ticks

// Configuration for Timer1
Timer1_ConfigType Timer1_config = {
		.initial_value = 0,
		.compare_value = 31250,
		.mode = COMPARE, // Compare mode
		.prescaler = PRESCALER_256 // Prescaler value
};

void recievePassword(uint8 *password);

/*
 * Description:
 * This function is used as a callback for Timer1.
 * It is called whenever Timer1 overflows or a compare match occurs.
 * It increments the volatile variable timer1_ticks.
 */
void timer1TickIncrement(void);

void main(void){
	uint8 password_buffer[PASSWORD_SIZE]; // Array to store password
	uint8 password_check_buffer[PASSWORD_SIZE];
	uint8 password_is_set_f;
	uint8 passwords_are_matched_f;
	uint8 check_is_set_temp;
	uint8 uart_command;
	uint8 password_temp_check;

	// UART Configuration
	UART_ConfigType UART_config = {
			.bit_data = EIGHT_BITS,
			.parity = NO_PARITY,
			.stop_bit = ONE_STOP_BIT,
			.baud_rate = 9600
	};

	TWI_ConfigType TWI_config = {
			.bit_rate = BAUDRATE_400KHZ,
			.address = 0xDA
	};

	SREG |= 1 << 7;

	UART_init(&UART_config);
	TWI_init(&TWI_config);
	DcMotor_Init();
	Buzzer_init();

	Timer1_setCallBack(timer1TickIncrement);

	EEPROM_readByte(IS_PASSWORD_SET_FLAG_LOCATION, &check_is_set_temp);
	if(check_is_set_temp != 1){
		EEPROM_writeByte(IS_PASSWORD_SET_FLAG_LOCATION,0);
		_delay_ms(15);
	}

	while(1){
		uart_command = UART_recieveByte();
		switch(uart_command){
		case IS_PASSWORD_SETTED:
			password_is_set_f = 0;
			EEPROM_readByte(IS_PASSWORD_SET_FLAG_LOCATION, &password_is_set_f);
			if(password_is_set_f){
				UART_sendByte(SETTED);
			}else{
				UART_sendByte(NOT_SETTED);
			}
			break;
		case GET_READY_FOR_PASSWORD:
			passwords_are_matched_f = 1;
			recievePassword(password_buffer);
			for(i_counter = 0; i_counter < PASSWORD_SIZE; i_counter++){
				EEPROM_readByte(i_counter,&password_temp_check);
				if(password_buffer[i_counter] != password_temp_check){
					passwords_are_matched_f = 0;
				}
				_delay_ms(15);
			}
			if(passwords_are_matched_f){
				UART_sendByte(CORRECT_PASSWORD);
			}else{
				UART_sendByte(NOT_CORRECT_PASSWORD);
			}
			break;
		case OPEN_DOOR:
			Timer1_init(&Timer1_config);
			DcMotor_Rotate(CW, FULL_SPEED);
			while(timer1_ticks != 15);
			timer1_ticks = 0;
			Timer1_deinit();

			Timer1_init(&Timer1_config);
			DcMotor_Rotate(STOP, ZERO_SPEED);
			while(timer1_ticks != 3);
			timer1_ticks = 0;
			Timer1_deinit();

			Timer1_init(&Timer1_config);
			DcMotor_Rotate(A_CW, FULL_SPEED);
			while(timer1_ticks != 15);
			timer1_ticks = 0;
			Timer1_deinit();

			DcMotor_Rotate(STOP, ZERO_SPEED);
			break;
		case ERROR_ACTION:
			Timer1_init(&Timer1_config);
			Buzzer_on();
			while(timer1_ticks != 60);
			timer1_ticks = 0;
			Timer1_deinit();
			Buzzer_off();
			break;
		case GET_READY_FOR_PASSWORD_ONE:
			recievePassword(password_buffer);
			break;
		case GET_READY_FOR_PASSWORD_TWO:
			recievePassword(password_check_buffer);
			break;
		case IS_MATCHED:
			passwords_are_matched_f = 1;

			for(i_counter = 0; i_counter < PASSWORD_SIZE; i_counter++){
				if(password_buffer[i_counter] != password_check_buffer[i_counter]){
					passwords_are_matched_f = 0;
					break;
				}
			}
			if(passwords_are_matched_f){
				UART_sendByte(MATCHED);

				EEPROM_writeByte(IS_PASSWORD_SET_FLAG_LOCATION, passwords_are_matched_f);
				_delay_ms(15);

				for(i_counter = 0; i_counter < PASSWORD_SIZE; i_counter++){
					EEPROM_writeByte(i_counter, password_buffer[i_counter]);
					_delay_ms(15);
				}
			}else{
				UART_sendByte(NOT_MATCHED);
			}

			_delay_ms(15);
			break;
		}

	}
}


void recievePassword(uint8 *password){
	for(i_counter = 0; i_counter < PASSWORD_SIZE; i_counter++){
		*(password+i_counter) = UART_recieveByte();
		_delay_ms(15);
	}
}

void timer1TickIncrement(void) {
    timer1_ticks++; // Increment the volatile variable timer1_ticks
}
