struct queue{
    struct tcb *head;
    struct tcb *tail;
	uint8_t q_size;
};

void init_queue(struct queue *q);
void enqueue(struct queue  *q, struct tcb *tcb);
struct tcb* dequeue(struct queue  *q);
void update_q(struct queue  *q, struct queue  *q_r, struct tcb *t, struct tcb *prev, uint8_t i, uint8_t q_size);
void update_q_wait(struct queue  *q_w, struct queue  *q_r);
void update_q_blocked(struct queue  *q_b, struct queue  *q_r);
void update_q_runnable(struct queue  *q);