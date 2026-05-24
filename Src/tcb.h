#ifndef TCB_H
#define TCB_H

typedef struct tcb {
	uint32_t *sp;
	uint8_t priority;

	struct tcb *next;
} tcb_t;

#endif
