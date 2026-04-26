#include <stdint.h>
#include <stm32l4xx.h>

#define SYSTICK_HZ 2

void delay(volatile uint32_t count) {
    while (count--) {
        __NOP();
    }
}

typedef enum state {
	BUSY,
	SLEEPING,
	IDLE
} state;

typedef struct process {
	void *sp; // stack pointer
	state proc_state;
	int priority;
	void (*fn_ptr)(void*);

	struct process* next; // next task
};

uint32_t stack_1[32];
uint32_t *sp1_ptr = &stack_1[32];

void init_stack(uint32_t *sp, void (*entry)(void), void *arg)
{
	*(--sp) = 0x01000000; // pXSR
	*(--sp) = (uint32_t)(entry); // pc
	*(--sp) = (uint32_t)0; // LR
	*(--sp) = (uint32_t)0; // R12
	*(--sp) = (uint32_t)0; // R3
	*(--sp) = (uint32_t)0; // R2
	*(--sp) = (uint32_t)0; // R1
	*(--sp) = (uint32_t)arg; // R0

	*(--sp) = (uint32_t)0; // R11
	*(--sp) = (uint32_t)0; // R10
	*(--sp) = (uint32_t)0; // R9
	*(--sp) = (uint32_t)0; // R8
	*(--sp) = (uint32_t)0; // R7
	*(--sp) = (uint32_t)0; // R6
	*(--sp) = (uint32_t)0; // R5
	*(--sp) = (uint32_t)0; // R4
}


process process_head = NULL;

void init_systick(int hz)
{
	SysTick->CTRL &= ~0x01;

	SysTick->LOAD = (4000000/hz)-1;

	SysTick->VAL &= ~0xFFF; // clear value reg

	SysTick->CTRL |= 0x04; // processor clock
	SysTick->CTRL |= 0x02; // enable exception

	SysTick->CTRL |= 0x01; //enable systick
}

void SysTick_Handler(void)
{
	// loop through list

	if (process_head == NULL)
	{
		return;
	}

	process tmp_process = process_head->next;
	next_running_process = tmp_process;
	while (tmp_process)
	{
		if (tmp_process->priority > next_running_process->priority && tmp_process->proc_state == BUSY)
		{
			continue;
		}
	}
}

int main(void)
{
    // 1. Enable clock for GPIOA
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Small delay after enabling clock (recommended)
    __NOP(); __NOP();

    // 2. Set PA5 as output (MODER5 = 01)
    GPIOA->MODER &= ~(3U << (5 * 2));   // clear bits
    GPIOA->MODER |=  (1U << (5 * 2));   // set as output

    // 3. Optional: push-pull (default), no pull-up/down
    GPIOA->OTYPER &= ~(1U << 5);
    GPIOA->PUPDR  &= ~(3U << (5 * 2));

    init_systick(SYSTICK_HZ);

    while (1)
    {
    	//GPIOA->ODR ^= (1U << 5);
    }
}
