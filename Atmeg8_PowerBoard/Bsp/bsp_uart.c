#include "bsp_uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "bsp.h"

#if defined (UART_TX_BY_INTERRUPT)
static uint8_t uart_tx_buf[UART_TX_BUF_SIZE];		/* 发送缓冲区 */
#endif
static uint8_t uart_rx_buf[UART_RX_BUF_SIZE];		/* 接受缓冲区 */
static uart_buf_manage_t buf_manage;
static uart_buf_manage_t* p_buf_manage = &buf_manage;

static void uart_init_buf(void)
{
#if defined (UART_TX_BY_INTERRUPT)
	p_buf_manage->p_txbuf = uart_tx_buf;
#endif
	p_buf_manage->p_rxbuf = uart_rx_buf;
#if defined (UART_TX_BY_INTERRUPT)
	p_buf_manage->txbuf_size = UART_TX_BUF_SIZE;
#endif
	p_buf_manage->rxbuf_size = UART_RX_BUF_SIZE;
#if defined (UART_TX_BY_INTERRUPT)
	p_buf_manage->tx_write = 0;
	p_buf_manage->tx_read = 0;
	p_buf_manage->tx_count = 0;
#endif
	p_buf_manage->rx_write = 0;
	p_buf_manage->rx_read = 0;
	p_buf_manage->rx_count = 0;
}

void uart_init(uint32_t baud_rate)
{
	uart_init_buf();
	
	uint8_t ubrr = (F_CPU / (16 * baud_rate)) - 1;
	/* Set baud rate */
	UBRRH = (unsigned char)(ubrr >> 8);
	UBRRL = (unsigned char)ubrr;
	/* Enable receiver, transmitter and RX Complete interrupt */
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
	/* Set frame format: 8data, 1stop bit */
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
}

#if 0
#pragma interrupt_handler uart_rx_isr:12
void uart_rx_isr(void)
{
	
}

#pragma interrupt_handler uart_tx_isr:13
void uart_tx_isr(void)
{
	
}
#else
ISR(USART_RXC_vect)
{
	if ((UCSRA & (1 << RXC)) > 0)
	{
		uint8_t ch;

		ch = UDR;
		p_buf_manage->p_rxbuf[p_buf_manage->rx_write] = ch;
		if (++p_buf_manage->rx_write >= p_buf_manage->rxbuf_size)
		{
			p_buf_manage->rx_write = 0;
		}
		if (p_buf_manage->rx_count < p_buf_manage->rxbuf_size)
		{
			p_buf_manage->rx_count++;
		}
	}
}

ISR(USART_UDRE_vect)
{
#if defined (UART_TX_BY_INTERRUPT)
	if ((UCSRA & (1 << UDRE)) > 0)
	{
		if (p_buf_manage->tx_count == 0)
		{
			UCSRB &= ~(1 << UDRIE);
			UCSRB |= (1 << TXCIE);
		}
		else
		{
			UDR = p_buf_manage->p_txbuf[p_buf_manage->tx_read];
			if (++p_buf_manage->tx_read >= p_buf_manage->txbuf_size)
			{
				p_buf_manage->tx_read = 0;
			}
			p_buf_manage->tx_count--;
		}

	}
#endif
}

ISR(USART_TXC_vect)
{
#if defined (UART_TX_BY_INTERRUPT)
	if ((UCSRA & (1 << TXC)) > 0)
	{
		if (p_buf_manage->tx_count == 0)
		{
			UCSRB &= ~(1 << TXCIE);
		}
		else
		{
			UDR = p_buf_manage->p_txbuf[p_buf_manage->tx_read];
			if (++p_buf_manage->tx_read >= p_buf_manage->txbuf_size)
			{
				p_buf_manage->tx_read = 0;
			}
			p_buf_manage->tx_count--;
		}
	}
#endif
}
#endif

void uart_send_buf(uint8_t *buf, uint16_t len)
{
	#if defined (UART_TX_BY_INTERRUPT)
	uint16_t i;

	for (i = 0; i < len; i++)
	{
		while (1)
		{
			volatile uint16_t usCount;

			DISABLE_INT();
			usCount = p_buf_manage->tx_count;
			ENABLE_INT();

			if (usCount < p_buf_manage->txbuf_size)
			{
				break;
			}
		}
		
		p_buf_manage->p_txbuf[p_buf_manage->tx_write] = buf[i];

		DISABLE_INT();
		if (++p_buf_manage->tx_write >= p_buf_manage->txbuf_size)
		{
			p_buf_manage->tx_write = 0;
		}
		p_buf_manage->tx_count++;
		ENABLE_INT();
	}

	UCSRB |= 1 << UDRIE;	//enable transmit interrupt enable
	#else
	uint16_t i = 0;
	for (i = 0; i < len; i++)
	{
		/* Wait for empty transmit buffer */
		while ( !( UCSRA & (1<<UDRE)) );
		/* Put data into buffer, sends the data */
		UDR = buf[i];
	}
	#endif
}

void uart_send_byte(uint8_t data)
{	
#if defined (UART_TX_BY_INTERRUPT)
	uart_send_buf(&data, 1);
#else
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
#endif
}

int8_t uart_get_byte(uint8_t *read_byte)
{
	uint16_t usCount;

	DISABLE_INT();
	usCount = p_buf_manage->rx_count;
	ENABLE_INT();

	if (usCount == 0)
	{
		return -1;
	}
	else
	{
		*read_byte = p_buf_manage->p_rxbuf[p_buf_manage->rx_read];

		DISABLE_INT();
		if (++p_buf_manage->rx_read >= p_buf_manage->rxbuf_size)
		{
			p_buf_manage->rx_read = 0;
		}
		p_buf_manage->rx_count--;
		ENABLE_INT();
		return 0;
	}
}