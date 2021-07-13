#include <stddef.h>
#include "kernel.h"
#include "queue.h"
#include "usart.h"

void init_queue(queue_t *q){
    q->head = NULL;
    q->tail = NULL;
	q->q_size = 0;
}

void enqueue(queue_t *q, tcb_t *tcb){
    if(q->tail != NULL){
        q->tail->next_tcb = tcb;
    }
    q->tail = tcb;
    if(q->head == NULL){
        q->head = tcb;
    }
	q->q_size += 1;
}

tcb_t* dequeue(queue_t *q){
    tcb_t *tmp = NULL;
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

void update_q(queue_t *q, queue_t *q_r, tcb_t *t, tcb_t *prev, uint8_t i, uint8_t q_size){
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

void update_q_blocked(queue_t *q_b, queue_t *q_r){
	if (q_b->q_size > 0){
		uint8_t q_size = q_b->q_size;
		uint8_t i = 0;
		tcb_t *t = q_b->head;
		tcb_t *prev = 0;
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

void update_q_wait(queue_t *q_w, queue_t *q_r){
	if (q_w->q_size > 0){
		uint8_t q_size = q_w->q_size;
		uint8_t i = 0;
		tcb_t *t = q_w->head;
		tcb_t *prev = 0;
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

void update_q_runnable(queue_t *q){
	uint8_t q_size = q->q_size;
	uint8_t i = 0;
	tcb_t *t = q->head;
	tcb_t *prev = 0;
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
