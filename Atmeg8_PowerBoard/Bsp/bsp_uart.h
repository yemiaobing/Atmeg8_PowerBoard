#ifndef BSP_UART_H_
#define BSP_UART_H_

#include <stdint.h>

//#define UART_TX_BY_INTERRUPT /* 使用发送中断，否则使用while(1)发送 */

/* 发送接收缓冲区大小定义 */
#if defined (UART_TX_BY_INTERRUPT)
#define UART_TX_BUF_SIZE	50
#endif
#define UART_RX_BUF_SIZE	50

typedef struct
{
#if defined (UART_TX_BY_INTERRUPT)
	uint8_t *p_txbuf;
#endif
	uint8_t *p_rxbuf;
#if defined (UART_TX_BY_INTERRUPT)
	uint16_t txbuf_size;
#endif
	uint16_t rxbuf_size;
#if defined (UART_TX_BY_INTERRUPT)
	volatile uint16_t tx_write;	
	volatile uint16_t tx_read;
	volatile uint16_t tx_count;
#endif

	volatile uint16_t rx_write;
	volatile uint16_t rx_read;
	volatile uint16_t rx_count;
}uart_buf_manage_t;

extern void uart_init(uint32_t baud_rate);
extern void uart_send_buf(uint8_t *buf, uint16_t len);
extern void uart_send_byte(uint8_t data);
extern int8_t uart_get_byte(uint8_t *read_byte);

#endif /* BSP_UART_H_ */