#include "buffer.h"

// Inicializacija globalnega bufferja
struct buffer_t buff = {0};

int BUFF_store_data(char data, struct buffer_t *buffer)
{
	// Preveri, �e je poln. Ja => return error
	if (buffer->size >= BUFFER_LENGTH)
	{
		return BUFFER_ERROR;
	}
	// Preveri, �e je prazen. Ja => postavi front in rear na 0
	else if (buffer->size == 0)
	{
		buffer->rear = 0;
		buffer->front = 0;
	}
	// Inkrementiraj, izvedi operacijo modulo
	else if (++(buffer->rear) >= BUFFER_LENGTH)
		buffer->rear = 0;

	// Zapi�i v buffer, inkrementiraj size
	buffer->buffer[buffer->rear] = data;
	buffer->size++;

	return BUFFER_OK;
}

int BUFF_get_data(struct buffer_t *buffer, char *data)
{
	// first check if buffer empty
	if (buffer->size == 0)
		return BUFFER_ERROR;

	// else read and return data from the buffer's front
	*data = buffer->buffer[buffer->front];

	//update the number of elements in the queue
	buffer->size--;

	// update the "front" value NOT using the modulo arithmetics but
	// using if-then instead because it is faster on uC
	buffer->front++;
	if (buffer->front >= BUFFER_LENGTH)
		buffer->front = 0;

	// return BUFFER_OK error code
	return BUFFER_OK;
}

uint16_t BUFF_get_number_of_items(struct buffer_t *buffer)
{
	return buffer->size;
}
