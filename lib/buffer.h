/*
 * Buffer lib 
 *	
 * 	Created: 16.8.2021
 *  Author: Kristjan Šoln
 */ 


#ifndef BUFFER_H_
#define BUFFER_H_

#include <avr/io.h>
#include <stdlib.h>

// settings
#define BUFFER_LENGTH	250		// buffer lenght (up to 255)
// return error values
#define BUFFER_OK		1		// no error return value
#define BUFFER_ERROR	0		// return value if error occurred

// Definicija strukture buffer
struct buffer_t
{
	volatile uint8_t buffer[BUFFER_LENGTH];	// 8 bit buffer cells
	volatile uint8_t front;		
	volatile uint8_t rear;
	volatile uint8_t size;
};



// function prototypes
int BUFF_store_data(char data, struct buffer_t *buffer);		// Store one byte of data (returns BUFFER_OK or BUFFER_ERROR)
int BUFF_get_data(struct buffer_t *buffer, char *data);			// Read one byte of data (returns BUFFER_OK or BUFFER_ERROR)
uint16_t BUFF_get_number_of_items(struct buffer_t *buffer);		// Get the number of bytes currently in the buffer


#endif /* BUFFER_H_ */