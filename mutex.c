/*
 * mutex.c
 *
 * Created: 21.07.2021 14:22:00
 *  Author: bursztyn
 */ 

#include "mutex.h"

void mutex_lock(struct mutex* mtx){
	mtx->state = MTX_LOCKED;
}

void mutex_unlock(struct mutex* mtx){
	mtx->state = MTX_UNLOCKED;
}

uint8_t mutex_state(struct mutex* mtx){
	return mtx->state;
}