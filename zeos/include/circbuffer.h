/* 
 * circbuffer.h - A circular buffer of 256 bytes
*/

#define CIRCBUFFER_MAX_CAPACITY 256

typedef struct {
	char buff[CIRCBUFFER_MAX_CAPACITY];	
	int head;	// index of the first element 
	int tail;	// index of the last element
	int size;	// current active bytes in buff
} Circbuffer;

extern Circbuffer c;

void start_circbuffer(Circbuffer *cb);

int isEmpty(Circbuffer *cb);

int store(Circbuffer *cb, char data);

int load(Circbuffer *cb, char *ret, int numchars);