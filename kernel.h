/*
 * kernel.h
 *
 * Created: 13.07.2021 16:54:37
 *  Author: bursztyn
 */ 


#ifndef KERNEL_H_
#define KERNEL_H_

#include <avr/io.h>
#include "mutex.h"

//task states
#define SUSPENDED				0
#define RESET					1
#define RUNNABLE				2
#define BLOCKED					3
#define DELAYED					4
#define USART_BLOCKED			5
#define SDS011_BLOCKED			6
#define TWI_BLOCKED				7
#define TMP102_BLOCKED			8
#define READY					9
#define RUNNING					10
#define SUSPENDED_K				11
#define FINISHED				12
#define WORK_F					13
#define WORK_S					14
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
#define M						3

//memory states
#define FREE					0
#define ALLOC					1

#define MAX_TASKS				6
#define DEF_MEM_SIZE			0x32

typedef void (*task_fun)(void);

uint8_t create_task(task_fun fun, uint8_t type, uint16_t delay, uint8_t mem_size);
uint8_t setup_task(uint8_t mem_size);
void task_block(uint8_t desc, struct mutex* mtx);
void check_overflow();

void init_kernel();
void start_kernel();
void init_drivers();
void task_suspend();

struct memory{
	void *sp;
	void* sp_start;
	uint8_t m_size;
	uint8_t m_state;
};

struct tcb{
	void* sp;
	void* sp_start;
	struct memory* mem;
	uint8_t id;
	uint8_t m_state;
	uint8_t state;
	uint8_t type;
	uint16_t delay;
	uint8_t timer;
	struct tcb* next_tcb;
	struct tcb* m_next_tcb;
	task_fun fun;
	uint8_t state_desc;
	uint8_t queue_state;
	uint8_t c_queue;
	uint8_t tick_count;
	uint8_t w_state;
	struct mutex* mtx;
};

struct tcb* get_current_tcb();
void task_notify(struct tcb *tcb);
struct memory* find_mem();

#endif /* KERNEL_H_ */