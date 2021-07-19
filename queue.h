#define DEFAULT		0
#define MASTER		1

struct queue{
    struct tcb *head;
    struct tcb *tail;
	uint8_t q_size;
	uint8_t type;
};

void init_queue(struct queue *q, uint8_t type);
void enqueue(struct queue  *q, struct tcb *tcb);
struct tcb* dequeue(struct queue  *q);
void update_q(struct queue  *q, struct queue  *q_r, struct tcb *t, struct tcb *prev, uint8_t i, uint8_t q_size);
void update_q_wait(struct queue  *q_w, struct queue  *q_r);
void update_q_blocked(struct queue  *q_b, struct queue  *q_r);
void update_q_runnable(struct queue  *q);
void update_q_master(struct queue *q_m, struct queue  *q_w, struct queue  *q_r);