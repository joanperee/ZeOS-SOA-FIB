#include <circbuffer.h>

/*
	The circular buffer we implemented has 4 attributes: the buffer itself (256 Bytes as for now),
	the head and tail indexs (to keep track of where to read and where to write/overwrite), and the
	current size of the buffer. It will be used in the project as the read function blocks the user
	expecting "maxchars" bytes. Basically it is a basic FIFO structure.
	Therefore, we will have the following methods:

	- start_circbuffer --> initializes the object as it can't be done in the struct declaration
	- store --> given a keyboard interrupt, stores the character inside the buffer and returns 0 if executed correctly
	- load --> given a circular buffer it puts on a buffer (param by reference) the first X bytes wanted by the user, 0 if executed correctly, -1 if invalid 
	- isEmpty --> returns true if the circular buffer c has size = 0.
*/
void start_circbuffer(Circbuffer *cb) {
	cb->head = 0;
	cb->tail = 0;
	cb->size = 0;
}

int isEmpty(Circbuffer *cb) {
	return cb->size == 0;
}

int store(Circbuffer *cb, char data) {
	// Store the byte in the tail index
	cb->buff[cb->tail] = data;

	// Increment the tail index inside the range of the buffer
	cb->tail = (cb->tail + 1) % CIRCBUFFER_MAX_CAPACITY;

	// Increment the size only if the buffer is not full
	if (cb->size < CIRCBUFFER_MAX_CAPACITY) cb->size++;
	else {
		// If the buffer is full the head must be incremented 
		// as it was overwritten by the tail
		cb->head = (cb->head + 1) % CIRCBUFFER_MAX_CAPACITY;
	}
	return 0;	
}

int load(Circbuffer *cb, char *ret, int numchars) {
    // If the circular buffer is empty then we cannot load any bytes inside *ret
	if (isEmpty(cb)) return -1;
	
	// Compute how many active chars we can really read 
    int toRead = (numchars < cb->size) ? numchars : cb->size;

	// Loop through all the chars within the load reach
    for (int i = 0; i < toRead; ++i) {
        ret[i] = cb->buff[cb->head];
        cb->head = (cb->head + 1) % CIRCBUFFER_MAX_CAPACITY;
        cb->size--;
    }
    return toRead;
}