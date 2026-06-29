

#include <stdlib.h>
#include "usart.h"
#include "assert.h"

static inline uint32_t get_uart_clk();
static void usart_handle_txe(usart_t *usart);
static void usart_handle_tc(usart_t *usart);
static inline uint32_t compute_USARTDIV(const uint32_t baudrate, USART_TypeDef *regs);
static void usart_irq(usart_t *usart);
static void usart_handle_rxne(usart_t *usart);

static usart_t *usart1;
static usart_t *usart2;
static usart_t *usart3;
static usart_t *usart4;

bool usart_init(usart_t *usart, USART_TypeDef *regs, const usart_config_t *config)
{
	if (!usart || !regs || !config)
	{
		return false;
	}

	regs->CR1 &= ~((1 << 0) | (1 << 12) | (1 << 28));
	regs->CR1 |= ((config->word_len & 0b01) << 12) | ((config->word_len & 0b10) << 28);

	regs->CR1 &= ~((1 << 10) | (1 << 9));
	if (config->parity_bit != UART_PARITY_NONE)
	{
		regs->CR1 |= ((1 << 10) | (config->parity_bit << 9));
	}

	regs->CR2 &= ~((0x3 << 12));
	regs->CR2 |= (config->stop_bits << 12);

	uint32_t usartdiv = compute_USARTDIV(config->baudrate, regs);

	regs->BRR = (usartdiv & 0x0000FFF0);

	if (regs->CR1 & (1 << 15))
	{
		// oversampling by 8
		regs->BRR |= (usartdiv & 0xF) >> 1;
	}
	else
	{
		regs->BRR |= usartdiv & 0xF;
	}

	// enable transmitter and reciever
	regs->CR1 |= (1 << 3) | (1 << 2);

	regs->CR1 |= (1 << 0); // USART enable
}

static inline uint32_t compute_USARTDIV(const uint32_t baudrate, USART_TypeDef *regs)
{
	/*
	 * calculation for baud rate:
	 * if oversampling by 16: f_ck/USARTDIV
	 * if oversampling by 8: 2*f_ck/USARTDIV
	 */

	if (regs->CR1 & (1 << 15))
	{
		// oversampling by 8
		return 2 * get_uart_clk() / baudrate;
	}

	return get_uart_clk() / baudrate;

}

static inline uint32_t get_uart_clk()
{
	return 16000000; // assumption for now, need to change later
}

// non-blocking
bool usart_write_async(usart_t *usart, const void *buf, size_t len)
{
	if (!usart || !buf || len <= 0)
	{
		return false;
	}

	/*
	 * logical flow: put data into transmit fifo
	 *				 enable transmit complete interrupt
	 *				 everytime interrupt goes off, load more data into transmit fifo
	 *				 when no more data left to load into fifo, auto disable transmit fifo from inside interrupt
	 */

	usart->tx.buf = (uint8_t *)buf;
	usart->tx.count = len;
	usart->tx.state = USART_BUSY;

	usart->regs->CR1 |= USART_CR1_TXEIE; // transmit interrupts enable

	// put data into TDR reg, and then when TC=1, load more data into the TDR reg

	// can only write the TDR reg when TXE=1 (transmit data register empty)

	// THERE ARE OTHER INTERRUPTS TO BE ENABLED, NEED TO LOOK INTO THIS MORE LATER
}

// non-blocking
bool usart_read_async(usart_t *usart, void *buf, size_t len)
{
	usart->rx.size = len;
	usart->rx.state = USART_BUSY;

	usart->rx.buf = &(buf[len-1]);

	usart->regs->CR1 |= USART_CR1_RXNEIE;
}

void USART1_IRQHandler()
{
	usart_irq(usart1);
}

static void usart_irq(usart_t *usart)
{
	ASSERT(usart != NULL);

	// grab read of the status reg
	uint32_t status = usart->regs->ISR;

	if (status & USART_ISR_TXE)
	{
		usart_handle_txe(usart);
	}

	if (status & USART_ISR_TC)
	{
		usart_handle_tc(usart);
	}

	if (status & USART_ISR_RXNE)
	{
		usart_handle_rxne(usart);
	}
}

static void usart_handle_txe(usart_t *usart)
{
	if (usart->tx.count > 0)
	{
		usart->regs->TDR = *(usart->tx.buf)++;
		usart->tx.count--;
	}
	else
	{
		usart->regs->CR1 &= ~USART_CR1_TXEIE; // disable transmit interrupts
		usart->regs->CR1 |= USART_CR1_TCIE; // enable transmit interrupt
	}
}

static void usart_handle_tc(usart_t *usart)
{
	usart->regs->CR1 &= ~USART_CR1_TCIE;
	usart->tx.state = USART_IDLE;
}

static void usart_handle_rxne(usart_t *usart)
{
		usart->buf = usart->regs->RDR;
		usart->buf--;
		usart->count--;

		if (usart->rx->count <= 0)
		{
			usart->regs->CR1 &= ~USART_CR1_RXNEIE;
			usart->rx.state = USART_IDLE;
		}
}



