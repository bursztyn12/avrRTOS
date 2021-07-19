#include <stddef.h>
#include "kernel.h"
#include "queue.h"
#include "usart.h"

void init_queue(struct queue  *q, uint8_t type){
    q->head = NULL;
    q->tail = NULL;
	q->q_size = 0;
	q->type = type;
}

void enqueue(struct queue  *q, struct tcb *tcb){
    if(q->tail != NULL){
        if (q->type == MASTER){
			q->tail->m_next_tcb = tcb;
        }else{
			q->tail->next_tcb = tcb;
		}
    }
    q->tail = tcb;
    if(q->head == NULL){
        q->head = tcb;
    }
	q->q_size += 1;
}

struct tcb* dequeue(struct queue  *q){
    struct tcb *h = NULL;
	if (q->head == NULL){
		return h;
    }
	h = q->head;
	
	if (q->type == MASTER){
		q->head = q->head->m_next_tcb;
	}else{
		q->head = h->next_tcb;
		h->next_tcb = NULL;
	}
	
	if (q->head == NULL){
		q->tail = NULL;
	}
	q->q_size -= 1;
    return h;
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
		uint8_t q_i = 0;
		uint8_t q_s = q_w->q_size;
		struct tcb *t = q_w->head;
		struct tcb *prev = 0;
		for(;i<q_size;i++){
			if (t->state != BLOCKED && t->state != SUSPENDED && t->c_queue == W){
				t->timer -= 1;
				if (t->timer == 0){
					if (t->type == PERODIC){
						t->timer = t->delay;
						t->state = DELAYED;
					}
					update_q(q_w, q_r, t, prev, q_i, q_s);
				}
			}
			prev = t;
			if(t->next_tcb == NULL){
				t = q_w->head;
				q_s = q_w->q_size;
				q_i = 0;
			}else{
				++q_i;
				t = t->next_tcb;
			}
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

void update_q_master(struct queue *q_m, struct queue  *q_w, struct queue  *q_r){
	uint8_t q_size = q_m->q_size;
	uint8_t i = 0;
	struct tcb *t;
	for(;i<q_size;i++){
		t = dequeue(q_m);
		if (t->c_queue == M){
			if(t->state == READY){
				t->state = RUNNABLE;
				t->c_queue = R;
				enqueue(q_r, t);
			}else if(t->state == SUSPENDED_K){
				if (t->m_state == DELAYED){
					t->c_queue = W;
					enqueue(q_w, t);
				}else{
					t->c_queue = R;
					enqueue(q_r, t);
				}
			}else if(t->state == DELAYED){
				t->c_queue = W;
				enqueue(q_w, t);
			}else if (t->state == FINISHED){
				if (t->m_state == DELAYED){
					t->c_queue = W;
					enqueue(q_w, t);
				}else if (t->m_state == READY && t->state != FINISHED){
					t->state = RESET;
					t->c_queue = R;
					enqueue(q_r, t);
				}
			}
		}
		if (t->type != SINGLE){
			enqueue(q_m, t);
		}else if(t->type == SINGLE && t->state != FINISHED){
			enqueue(q_m, t);
		}
	}
}