/*
 * usart.h
 *
 *  Created on: Jun 21, 2026
 *      Author: Matthew Sundquist
 */

#ifndef USART_H_
#define USART_H_

#include <stm32l4xx.h>
#include <stdint.h>
#include <stdbool.h>

// DO NOT RESET TE BIT WHILE TRANSMITTING

typedef enum {
	UART_WORD_LEN_8 = 0,
	UART_WORD_LEN_9,
	UART_WORD_LEN_7
} usart_word_len_t;

typedef enum {
	UART_STOP_BITS_1,
	UART_STOP_BITS_0_5, // default
	UART_STOP_BITS_2,
	UART_STOP_BITS_1_5
} usart_stop_bits_t;

typedef enum {
	UART_PARITY_EVEN = 0,
	UART_PARITY_ODD,
	UART_PARITY_NONE
} usart_parity_bit_t;

typedef enum {
	USART_BUSY,
	USART_IDLE
} usart_state_t;


typedef struct
{
	uint8_t *buf;
	size_t count;
	uint8_t write_idx;

	usart_state_t state;

} usart_channel_t;

typedef struct usart_config
{
	uint32_t baudrate;
	usart_word_len_t word_len;
	usart_stop_bits_t stop_bits;
	usart_parity_bit_t parity_bit;
} usart_config_t;

// uart behavior: data shifted out by the least significant bit

// default word length of 8 bits
typedef struct usart {
	USART_TypeDef *regs; // contains all registers needed

	usart_config_t config;

	usart_channel_t rx;
	usart_channel_t tx;
} usart_t;


bool usart_init(usart_t *usart, USART_TypeDef *regs, const usart_config_t *config);

// non-blocking
bool usart_write_async(usart_t *usart, const void *buf, size_t len);

// non-blocking
bool usart_read_async(usart_t *usart, void *buf, size_t len);


#endif /* USART_H_ */
