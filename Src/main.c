#include <stdint.h>
#include <stm32l4xx.h>
#include <stdlib.h>
#include "scheduler.h"
#include "task.h"
#include "config.h"
#include "assert.h"
#include "string.h"
#include "mutex.h"
#include "usart.h"

#ifdef UNIT_TESTS
#include "unit_tests.h"
uint32_t num_errors = 0;
#endif

//#define UNIT_TESTS
#define SYSTICK_HZ 10

volatile uint8_t os_started = 0; // 0 for not running, 1 for running
int cur_task = 2;

void delay(volatile uint32_t count) {
    while (count--) {
        __NOP();
    }
}


__attribute__((aligned(8))) uint32_t task_1_stack[STACK_SIZE];
__attribute__((aligned(8))) uint32_t task_2_stack[STACK_SIZE];
__attribute__((aligned(8))) uint32_t idle_stack[STACK_SIZE];

uint32_t *task_1_sp = &task_1_stack[STACK_SIZE];
uint32_t *task_2_sp = &task_2_stack[STACK_SIZE];
uint32_t *idle_task_sp = &idle_stack[STACK_SIZE];

tcb_t task_1;

tcb_t task_2;

tcb_t idle_task;

uint32_t arg_1 = 0;
uint32_t arg_2 = 0;
uint32_t arg_3 = 0;

mutex_t test_mutex;

uint32_t c1 = 0;
uint32_t c2 = 0;
uint32_t shared_c = 0;


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
}

void turn_on_LED(void)
{
	scheduler_t *s = get_scheduler();
	while (1)
	{
		mutex_aquire(&test_mutex);

		GPIOA->ODR = (1U << 5);

		mutex_release(&test_mutex);
	}
}

void turn_off_LED(void)
{
	scheduler_t *s = get_scheduler();
	while (1)
	{
		mutex_aquire(&test_mutex);

		GPIOA->ODR = (0U << 5);

		task_sleep(30);

		mutex_release(&test_mutex);

		task_sleep(10);
	}
}

void idle_task_func(void)
{
	while (1)
	{
		__NOP();
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
int main(void)
{
	task_queue_tests();
	scheduler_tests();
	while (1)
	{
		__BKPT(0); // passed
	}
}
#else
int main(void)
{

	//init_scheduler();

	//mutex_init(&test_mutex);
	// Disable FPU (CP10 and CP11 Full Access clear)
	// This forces the CPU to use standard 8-word hardware stacking
	SCB->CPACR &= ~((3UL << 20) | (3UL << 22));

//	int status = task_create(&task_1, turn_on_LED, &arg_1, 1, task_1_sp, "LED_on");
//	status += task_create(&task_2, turn_off_LED, &arg_2, 1, task_2_sp, "LED_off");
//	status += task_create(&idle_task, idle_task_func, &arg_3, 0, idle_task_sp, "IDLE");

//	ASSERT(status == 0);

	//gpio_setup();

//    init_systick(SYSTICK_HZ); // enables the scheduler

	uint8_t rx_buf[100];
	uint8_t tx_buf[100];

	USART_TypeDef *usart1_regs = USART1;
	usart_t usart1_inst;
	usart_config_t usart1_config = {
			.baudrate = 9600,
			.word_len = USART_WORD_LEN_8,
			.stop_bits = UART_STOP_BITS_1,
			.parity_bit = UART_PARITY_NONE,

			.tx_buf = {
					.buf = tx_buf,
					.size = sizeof(tx_buf)
			},
			.rx_buf = {
					.buf = rx_buf,
					.size = sizeof(rx_buf)
			}
	};

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	(void)RCC->AHB2ENR;

	// PA9, PA10 -> AF mode
	GPIOA->MODER &= ~((3 << 18) | (3 << 20));
	GPIOA->MODER |=  ((2 << 18) | (2 << 20));

	// AF7 = USART1
	GPIOA->AFR[1] &= ~((0xF << 4) | (0xF << 8));
	GPIOA->AFR[1] |=  ((7 << 4) | (7 << 8));

	NVIC_EnableIRQ(USART1_IRQn);

	usart_init(&usart1_inst, usart1_regs, &usart1_config);

	int entry = 1;
	while (1)
	{
		uint8_t recv_buf[5];
		uint8_t buf[5] = {1, 2, 3, 4, 5};
		if (entry == 1)
		{
			usart_read_async(&usart1_inst, recv_buf, sizeof(recv_buf));

			usart_write_async(&usart1_inst, buf, sizeof(buf));


			entry = 0;
		}
		if (recv_buf[0] == 's' && recv_buf[4] == 'a')
		{
			__BKPT(0);
		}
	}
}
#endif
