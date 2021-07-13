typedef struct queue_t{
    tcb_t *head;
    tcb_t *tail;
	uint8_t q_size;
}queue_t;

void init_queue(queue_t *q);
void enqueue(queue_t *q, tcb_t *tcb);
tcb_t* dequeue(queue_t *q);
void update_q(queue_t *q, queue_t *q_r, tcb_t *t, tcb_t *prev, uint8_t i, uint8_t q_size);
void update_q_wait(queue_t *q_w, queue_t *q_r);
void update_q_blocked(queue_t *q_b, queue_t *q_r);
void update_q_runnable(queue_t *q);