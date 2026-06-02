#include <stdint.h>
#include <stm32l4xx.h>
#include <stdlib.h>
#include "scheduler.h"
#include "task_stack.h"

//#define UNIT_TESTS
#define SYSTICK_HZ 10
#define STACK_SIZE 64

volatile uint8_t os_started = 0; // 0 for not running, 1 for running
int cur_task = 2;

void delay(volatile uint32_t count) {
    while (count--) {
        __NOP();
    }
}


__attribute__((aligned(8))) uint32_t task_1_stack[STACK_SIZE];
__attribute__((aligned(8))) uint32_t task_2_stack[STACK_SIZE];

uint32_t *task_1_sp = &task_1_stack[STACK_SIZE];
uint32_t *task_2_sp = &task_2_stack[STACK_SIZE];

tcb_t task_1;

tcb_t task_2;

uint32_t arg_1 = 0;
uint32_t arg_2 = 0;

void init_systick(int hz)
{
	SysTick->CTRL &= ~0x01;

	SysTick->LOAD = (4000000/hz)-1;

	SysTick->VAL &= ~0xFFF; // clear value reg

	SysTick->CTRL |= 0x04; // processor clock
	SysTick->CTRL |= 0x02; // enable exception

	SysTick->CTRL |= 0x01; //enable systick
}

void SysTick_Handler()
{
	scheduler_tick();
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; // deferred PendSV interrupt
}

void turn_on_LED(void)
{
	while (1)
	{
		GPIOA->ODR = (1U << 5);
	}
}

void turn_off_LED(void)
{
	while (1)
	{
		GPIOA->ODR = (0U << 5);
	}
}

void gpio_setup(void)
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
}


#ifdef UNIT_TESTS

#include "unit_tests.h"
int main(void)
{
	ready_lists_tests();
	scheduler_tests();
	while (1)
	{
		__BKPT(0); // passed
	}
}
#else
int main(void)
{
//	process task_1;
//	process task_2;
//
//	context_switch(&task_1, &task_2);

	init_scheduler(5);

	// Disable FPU (CP10 and CP11 Full Access clear)
	// This forces the CPU to use standard 8-word hardware stacking
	SCB->CPACR &= ~((3UL << 20) | (3UL << 22));

	task_1.sp = init_stack(task_1_sp, turn_on_LED, &arg_1);
	task_2.sp = init_stack(task_2_sp, turn_off_LED, &arg_2);

	task_1.priority = 1;
	task_2.priority = 1;

	task_add_ready(&task_1);
	task_add_ready(&task_2);

	gpio_setup();

    init_systick(SYSTICK_HZ); // enables the scheduler

    while (1)
    {
    	//GPIOA->ODR ^= (1U << 5);
    }
}
#endif
