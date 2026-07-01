

#include <stdlib.h>
#include "usart.h"
#include "assert.h"

static inline uint32_t get_uart_clk();
static void usart_handle_txe(usart_t *usart);
static void usart_handle_tc(usart_t *usart);
static inline uint32_t compute_USARTDIV(const uint32_t baudrate, USART_TypeDef *regs);
static void usart_irq(usart_t *usart);
static void usart_handle_rxne(usart_t *usart);
static void usart_handle_ore(usart_t *usart);
static void usart_handle_idle(usart_t *usart);
static inline bool set_rx_int(usart_t *usart, bool enabled);

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

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // need to change to not be hardcoded

	usart->regs = regs;
	usart->config = config;

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

	usart1 = usart;

	ring_buffer_init(&usart->rx.rb, usart->config->rx_buf.buf, usart->config->rx_buf.size);
	ring_buffer_init(&usart->tx.rb, usart->config->tx_buf.buf, usart->config->tx_buf.size);

	usart->rx.num_errors = 0;
	usart->tx.num_errors = 0;

	return true;
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

	if (!ring_buffer_push(&usart->tx.rb, (uint8_t *)buf, len))
	{
		return false;
	}

	usart->tx.state = USART_BUSY;

	usart->regs->CR1 |= USART_CR1_TXEIE; // transmit interrupts enable
	usart->regs->CR1 &= ~USART_CR1_TCIE; // disable transmit complete interrupt

	return true;
}

// non-blocking
bool usart_read_async(usart_t *usart, void *buf, size_t len)
{
	if (!usart || !buf || len <= 0)
	{
		return false;
	}

	if (ring_buffer_get_count(&usart->rx.rb) < len)
	{
		return false;
	}

	if (ring_buffer_pop(&usart->rx.rb, (uint8_t *) buf, len))
	{
		return false;
	}

	return true;
}

bool usart_enable_rx_int(usart_t *usart)
{
	return set_rx_int(usart, true);
}

bool usart_disable_rx_int(usart_t *usart)
{
	return set_rx_int(usart, false);
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

	if (status & USART_ISR_RXNE)
	{
		usart_handle_rxne(usart);
	}

	if (status & USART_ISR_TXE)
	{
		usart_handle_txe(usart);
	}

	if (status & USART_ISR_TC)
	{
		usart_handle_tc(usart);
	}

	if (status & USART_ISR_ORE)
	{
		usart_handle_ore(usart);
	}

	if (status & USART_ISR_IDLE)
	{
		usart_handle_idle(usart);
	}
}

static void usart_handle_txe(usart_t *usart)
{
	if (!ring_buffer_pop_byte(&usart->tx.rb, (uint8_t *)&usart->regs->TDR))
	{
		usart->tx.num_errors++;
		return;
	}

	if (ring_buffer_empty(&usart->tx.rb))
	{
		usart->regs->CR1 &= ~USART_CR1_TXEIE; // disable transmit interrupts
		usart->regs->CR1 |= USART_CR1_TCIE; // enable transmit complete interrupt
	}
}

static void usart_handle_tc(usart_t *usart)
{
	usart->regs->CR1 &= ~USART_CR1_TCIE;
	usart->regs->ICR |= USART_ICR_TCCF;
	usart->tx.state = USART_IDLE;
}

static void usart_handle_rxne(usart_t *usart)
{
	if (!ring_buffer_push_byte(&usart->rx.rb, (uint8_t) (usart->regs->RDR & 0xFF)))
	{
		usart->rx.num_errors++;
		return;
	}
}

static void usart_handle_ore(usart_t *usart)
{
	// have to read the rdr to get rid of overrun error
	volatile uint8_t tmp = usart->regs->RDR & 0xF;
	(void) tmp;
	usart->regs->ICR = USART_ICR_ORECF;

	usart->rx.num_errors++;
	// maybe report error here
}

static void usart_handle_idle(usart_t *usart)
{
	(void)usart->regs->ISR;
	(void)usart->regs->RDR;
	usart->regs->ICR = USART_ICR_IDLECF;
}

static inline bool set_rx_int(usart_t *usart, bool enabled)
{
	if (!usart)
	{
		return false;
	}

	if (enabled)
	{
		usart->regs->CR1 |= USART_CR1_RXNEIE;
	}
	else
	{
		usart->regs->CR1 &= ~USART_CR1_RXNEIE;
	}

	return true;

	//usart->regs->CR1 = (usart->regs->CR1 & ~USART_CR1_RXNEIE) | (enabled * USART_CR1_RXNEIE);
}




