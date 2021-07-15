/*
 * kernel.c
 *
 * Created: 05.07.2021 12:19:49
 *  Author: bursztyn
 */ 

#define F_CPU 1000000U

#include <avr/interrupt.h>
#include <util/delay.h>

#include "kernel.h"
#include "queue.h"
#include "usart.h"
#include "twi.h"

uint8_t tcb_idx = 0;
static struct tcb a_tcb[MAX_TASKS];

void* start = (void *)RAMEND-0x20;
void *_p = 0;
void *_c_sp = 0;

static void scheduler(void);
task_fun sched = scheduler;

struct queue  q_runnable;
struct queue  q_wait;
struct queue  q_blocked;
struct queue  q_suspended;

volatile uint8_t k_tick = 1;

void kernel_idle(void){
	while(1){
		_delay_ms(12);
	}
}

static void init_sp(task_fun f){
	__asm__ volatile(
	//store current stack pointer
	"in	r30, __SP_L__				\n\t"
	"in	r31, __SP_H__				\n\t"
	
	//load new stack pointer for task space
	"out	__SP_L__, %A0				\n\t"
	"out	__SP_H__, %B0				\n\t"
	
	//early return address to scheduler if finished before kernel tick
	"push	%A2					\n\t"
	"push	%B2					\n\t"
	
	//push task function return address
	"push	%A1					\n\t"
	"push	%B1					\n\t"
	
	"ldi	r19, 0					\n\t"
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0
	"push	r19					\n\t" //r0

	//save task stack pointer
	"ldi   r26, lo8(_p)				\n\t"
	"ldi   r27, hi8(_p)				\n\t"
	"in    r0, __SP_L__         			\n\t"
	"st    x+, r0               			\n\t"
	"in    r0, __SP_H__         			\n\t"
	"st    x+, r0               			\n\t"
	
	//back to init_task stack pointer
	"out	__SP_L__, r30		\n\t"
	"out	__SP_H__, r31		\n\t"
	:"=r"(start), "=r"(f), "=r"(sched)
	:"0"(start), "1"(f), "2"(sched)
	);
}

struct tcb *tcb;

static void context_restore() __attribute__((naked));
static void context_restore(){
	__asm__(
	"cli						 \n\t"
	"ldi   r26, lo8(_c_sp)		 \n\t"
	"ldi   r27, hi8(_c_sp)		 \n\t"
	"ld r0, x+					 \n\t"
	"out __SP_L__, r0			 \n\t"
	"ld r0, x+					 \n\t"
	"out __SP_H__, r0		     \n\t"
	
	"pop	r31					 \n\t"
	"pop	r30					 \n\t"
	"pop	r29					 \n\t"
	"pop	r28					 \n\t"
	"pop	r27					 \n\t"
	"pop	r26					 \n\t"
	"pop	r25					 \n\t"
	"pop	r24					 \n\t"
	"pop	r23					 \n\t"
	"pop	r22					 \n\t"
	"pop	r21					 \n\t"
	"pop	r20					 \n\t"
	"pop	r19					 \n\t"
	"pop	r18					 \n\t"
	"pop	r17					 \n\t"
	"pop	r16					 \n\t"
	"pop	r15					 \n\t"
	"pop	r14					 \n\t"
	"pop	r13					 \n\t"
	"pop	r12					 \n\t"
	"pop	r11					 \n\t"
	"pop	r10					 \n\t"
	"pop	r9					 \n\t"
	"pop	r8					 \n\t"
	"pop	r7					 \n\t"
	"pop	r6					 \n\t"
	"pop	r5					 \n\t"
	"pop	r4					 \n\t"
	"pop	r3					 \n\t"
	"pop	r2					 \n\t"
	"pop	r1					 \n\t"
	"pop	r0					 \n\t"
	"sei						 \n\t"
	"reti						 \n\t"
	);
}

static inline void dispatch(void) __attribute__((always_inline));
static inline void dispatch(void){
	if (tcb->state == RESET){
		start = tcb->sp_start;
		init_sp(tcb->fun);
		tcb->sp = _p;
		_c_sp = tcb->sp;
	}
	
	if (tcb->type == PERODIC && tcb->state != BLOCKED){
		tcb->state = RESET;
		tcb->c_queue = W;
		enqueue(&q_wait, tcb);
	}
	
	_c_sp = tcb->sp;
	
	TCNT1 = 0x00;
	TIMSK = (1 << OCIE1A);
	
	context_restore();
}

static void scheduler(void){
	if (k_tick){
		update_q_wait(&q_wait, &q_runnable);
		update_q_blocked(&q_blocked, &q_runnable);
		//update_q_runnable(&q_runnable);
	}
	
	k_tick = 0;
	
	if (q_runnable.q_size > 0){
		tcb = dequeue(&q_runnable);
	}else{
		tcb = &a_tcb[0];
	}
	
	TIMSK &= ~(1 << OCIE1A);
	
	dispatch();
}

void initTimer(){
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); //set CTC mode, set prescaler to 1024
	TCNT1 = 0x00; //set count
	OCR1A = 0xA; //compare value (10 ms)
}

static inline void context_save() __attribute__((always_inline));
static inline void context_save(){
	__asm__ volatile(
	"cli						\n\t"
	"push	r0					\n\t"
	"push	r1					\n\t"
	"clr    r1					\n\t"
	"push	r2					\n\t"
	"push	r3					\n\t"
	"push	r4					\n\t"
	"push	r5					\n\t"
	"push	r6					\n\t"
	"push	r7					\n\t"
	"push	r8					\n\t"
	"push	r9					\n\t"
	"push	r10					\n\t"
	"push	r11					\n\t"
	"push	r12					\n\t"
	"push	r13					\n\t"
	"push	r14					\n\t"
	"push	r15					\n\t"
	"push	r16					\n\t"
	"push	r17					\n\t"
	"push	r18					\n\t"
	"push	r19					\n\t"
	"push	r20					\n\t"
	"push	r21					\n\t"
	"push	r22					\n\t"
	"push	r23					\n\t"
	"push	r24					\n\t"
	"push	r25					\n\t"
	"push	r26					\n\t"
	"push	r27					\n\t"
	"push	r28					\n\t"
	"push	r29					\n\t"
	"push	r30					\n\t"
	"push	r31					\n\t"
	
	"ldi   r26, lo8(_c_sp)		\n\t"
	"ldi   r27, hi8(_c_sp)		\n\t"
	"in    r0, __SP_L__         \n\t"
	"st    x+, r0               \n\t"
	"in    r0, __SP_H__         \n\t"
	"st    x+, r0               \n\t"
	
	"out	__SP_L__, %A0		\n\t"
	"out	__SP_H__, %B0		\n\t"
	"sei						\n\t"
	:: "z" (RAMEND)
	);
	tcb->sp = _c_sp;
}

ISR(TIMER1_COMPA_vect, ISR_NAKED){
	context_save();
	if (tcb->state != RESET && tcb->state != TERMINATED){
		tcb->sp = _c_sp;
		
		if (tcb->type != KERNEL){
			if (tcb->state != BLOCKED && tcb->state != SUSPENDED){
				tcb->state = SUSPENDED;
			}
			enqueue(&q_runnable, tcb);
		}
	}
	
	k_tick = 1;
	
	tcb->tick_count += 1;
	
	__asm__("ijmp				\n\t" :: "z"(scheduler));
}

void task_block(uint8_t desc) __attribute__((naked));
void task_block(uint8_t desc){
	context_save();
	tcb->state = BLOCKED;
	tcb->state_desc = desc;
	enqueue(&q_blocked, tcb);
	
	__asm__("ijmp				\n\t" :: "z"(scheduler));
}

void task_suspend() __attribute__((naked));
void task_suspend(){
	context_save();
	
	tcb->state = WAITING;
	
	__asm__("ijmp				\n\t" :: "z"(scheduler));
}

void task_notify(struct tcb *t){
	t->state = RUNNABLE;
	
	enqueue(&q_runnable, t);
}

void init_drivers(){
	DDRA = 0xFF;
	initTimer();
	usart_init();
	twi_init();
}

struct tcb* get_current_tcb(){
	return tcb;
}

uint8_t setup_task(){
	if (tcb_idx < MAX_TASKS){
		init_sp(a_tcb[tcb_idx].fun);
		a_tcb[tcb_idx].sp = _p;
		a_tcb[tcb_idx].sp_start = start;
		start -= 0x40;
		
		a_tcb[tcb_idx].id = tcb_idx;
		
		if (a_tcb[tcb_idx].type != KERNEL){
			if (a_tcb[tcb_idx].timer == 0){
				a_tcb[tcb_idx].state = RUNNABLE;
				a_tcb[tcb_idx].c_queue = R;
				enqueue(&q_runnable, &a_tcb[tcb_idx]);
			}else{
				a_tcb[tcb_idx].state = WAITING;
				a_tcb[tcb_idx].c_queue = W;
				enqueue(&q_wait, &a_tcb[tcb_idx]);
			}
		}
		
		++tcb_idx;
		return 0;
	}
	return 1;
}

uint8_t create_task(task_fun fun, uint8_t type, uint16_t delay){
	if (type == PERODIC){
		a_tcb[tcb_idx].type = PERODIC;
		a_tcb[tcb_idx].delay = delay;
	}else if(type == SINGLE){
		a_tcb[tcb_idx].type = SINGLE;
	}else if(type == FOREVER){
		a_tcb[tcb_idx].type = FOREVER;
	}else{
		return 1;
	}
	
	a_tcb[tcb_idx].fun = fun;
	a_tcb[tcb_idx].timer = delay;
	
	return setup_task();
}


void init_kernel(){
	init_drivers();
	
	init_queue(&q_runnable);
	init_queue(&q_wait);
	init_queue(&q_blocked);
	
	a_tcb[tcb_idx].fun = kernel_idle;
	a_tcb[tcb_idx].type = KERNEL;
	setup_task();
}

void start_kernel(){
	scheduler();
}
