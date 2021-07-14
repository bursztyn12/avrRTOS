#include <stddef.h>
#include "kernel.h"
#include "queue.h"
#include "usart.h"

void init_queue(struct queue  *q){
    q->head = NULL;
    q->tail = NULL;
	q->q_size = 0;
}

void enqueue(struct queue  *q, struct tcb *tcb){
    if(q->tail != NULL){
        q->tail->next_tcb = tcb;
    }
    q->tail = tcb;
    if(q->head == NULL){
        q->head = tcb;
    }
	q->q_size += 1;
}

struct tcb* dequeue(struct queue  *q){
    struct tcb *tmp = NULL;
	if (q->head == NULL){
		return tmp;
    }
	tmp = q->head;

	if (tmp->c_queue == tmp->next_tcb->c_queue){
		q->head = q->head->next_tcb;
	}else{
		q->head = NULL;
	}
	
	if (q->head == NULL){
		q->tail = NULL;
	}
	q->q_size -= 1;
    return tmp;
}

void update_q(struct queue  *q, struct queue  *q_r, struct tcb *t, struct tcb *prev, uint8_t i, uint8_t q_size){
	if (i > 0){
		if (i < q_size - 1){
			prev->next_tcb = t->next_tcb;
		}else{
			q->tail = prev;
		}
		q->q_size -= 1;
		enqueue(q_r, t);
		t->c_queue = R;
	}else{
		dequeue(q);
		enqueue(q_r, t);
		t->c_queue = R;
	}
	t->next_tcb = NULL;
}

void update_q_blocked(struct queue  *q_b, struct queue  *q_r){
	if (q_b->q_size > 0){
		uint8_t q_size = q_b->q_size;
		uint8_t i = 0;
		struct tcb *t = q_b->head;
		struct tcb *prev = 0;
		for(;i<q_size;i++){
			if (t->state_desc == USART_BLOCKED){
				if (get_usart_state()){
					update_q(q_b, q_r, t, prev, i, q_size);
				}
			}
			prev = t;
			t = t->next_tcb;
		}
	}
}

void update_q_wait(struct queue  *q_w, struct queue  *q_r){
	if (q_w->q_size > 0){
		uint8_t q_size = q_w->q_size;
		uint8_t i = 0;
		struct tcb *t = q_w->head;
		struct tcb *prev = 0;
		for(;i<q_size;i++){
			if (t->state != BLOCKED && t->state != SUSPENDED){
				t->timer -= 1;
				if (t->timer == 0){
					t->timer = t->delay;
					t->state = RESET;
					update_q(q_w, q_r, t, prev, i, q_size);
				}
			}
			prev = t;
			t = t->next_tcb;
		}
	}
}

void update_q_runnable(struct queue  *q){
	uint8_t q_size = q->q_size;
	uint8_t i = 0;
	struct tcb *t = q->head;
	struct tcb *prev = 0;
	for(;i<q_size;i++){
		if(t->state == TERMINATED){
			if (i > 0){
				if (i < q_size - 1){
					prev->next_tcb = t->next_tcb;
				}else{
					q->tail = prev;
				}
				q->q_size -= 1;
			}else{
				dequeue(q);
			}
			t->next_tcb = NULL;
		}
		prev = t;
		t = t->next_tcb;
	}
}
