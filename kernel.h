/*
 * kernel.h
 *
 * Created: 13.07.2021 16:54:37
 *  Author: bursztyn
 */ 


#ifndef KERNEL_H_
#define KERNEL_H_

#include <avr/io.h>

#define INIT					0
#define REFRESH					1

//task states
#define SUSPENDED				0
#define RESET					1
#define RUNNABLE				2
#define BLOCKED					3
#define WAITING					4
#define USART_BLOCKED			5
#define SDS011_BLOCKED			6
#define TWI_BLOCKED				7
#define TMP102_BLOCKED			8
#define TERMINATED				50

//task types
#define SINGLE					0
#define FOREVER					1
#define PERODIC					2
#define KERNEL					3

//current queue
#define R						0
#define W						1
#define B						2

#define MAX_TASKS				6

typedef void (*task_fun)(void);

uint8_t create_task(task_fun fun, uint8_t type, uint16_t delay);
uint8_t setup_task();
void task_block(uint8_t desc);
void task_suspend();

void init_kernel();
void start_kernel();
void init_drivers();


struct tcb{
	void* sp;
	void* sp_start;
	uint8_t id;
	uint8_t state;
	uint8_t type;
	uint16_t delay;
	uint8_t timer;
	struct tcb* next_tcb;
	task_fun fun;
	uint8_t state_desc;
	uint8_t queue_state;
	uint8_t c_queue;
	uint8_t tick_count;
};

struct tcb* get_current_tcb();
void task_notify(struct tcb *tcb);

#define K_TCB_DEFINE_SIGNLE(_f) \
{													\
	.state = RUNNABLE,								\
	.type = SINGLE,									\
	.delay = 0,										\
	.timer = 0,										\
	.fun = _f,										\
}

#define K_TCB_DEFINE_PERODIC(_f, _delay) \
{													\
	.state = RUNNABLE,								\
	.type = PERODIC,								\
	.delay = _delay,								\
	.timer = _delay,								\
	.fun = _f,										\
}

#define K_TCB_DEFINE_FOREVER(_f) \
{													\
	.state = RUNNABLE,								\
	.type = FOREVER,								\
	.delay = 0,										\
	.timer = 0,										\
	.fun = _f,										\
}

#endif /* KERNEL_H_ */