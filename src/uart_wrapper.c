#include <libopencm3/stm32/usart.h>
#include "uart.h"

uint32_t ow_uart;

int uart_init(char *dev_path)
{
	ow_uart = (uint32_t) dev_path;
	usart_set_databits(ow_uart, 8);
	usart_set_parity(ow_uart, USART_PARITY_NONE);
	usart_set_stopbits(ow_uart, USART_STOPBITS_1);
	usart_set_flow_control(ow_uart, USART_FLOWCONTROL_NONE);
	usart_set_mode(ow_uart, USART_MODE_TX_RX);
	usart_enable(ow_uart);
	return 0;
}

void uart_finit(void)
{
	usart_disable(ow_uart);
	ow_uart = 0;
}

void uart_setb(uint32_t baud)
{
	usart_set_baudrate(ow_uart, baud);
}

void uart_putc(unsigned char c)
{
	usart_send_blocking(ow_uart, c);
}

unsigned char uart_getc(void)
{
	return usart_recv_blocking(ow_uart);
}

