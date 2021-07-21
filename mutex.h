/*
 * mutex.h
 *
 * Created: 21.07.2021 14:21:43
 *  Author: bursztyn
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_

#include <avr/io.h>

//mutex states
#define MTX_UNLOCKED		0
#define MTX_LOCKED			1

struct mutex{
	uint8_t state;	
};

void mutex_lock(struct mutex* mtx);
void mutex_unlock(struct mutex* mtx);
uint8_t mutex_state(struct mutex* mtx);

#endif /* MUTEX_H_ */