#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>

#include <cmrx/os/sched.h>

#undef SEMIHOSTING

#define USART1_TX_PORT		GPIOA
#define USART1_TX_PIN		GPIO9

#define USART1_RX_PORT		GPIOA
#define USART1_RX_PIN		GPIO10

#define USART2_TX_PORT		GPIOA
#define USART2_TX_PIN		GPIO2

#define USART2_RX_PORT		GPIOA
#define USART2_RX_PIN		GPIO3

void clock_setup(void)
{
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_USART2);
}

void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);

	// USART1 RX & RX
	gpio_mode_setup(USART1_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART1_TX_PIN);
	gpio_mode_setup(USART1_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART1_RX_PIN);

	gpio_mode_setup(USART2_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART2_TX_PIN);
	gpio_mode_setup(USART2_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART2_RX_PIN);


	gpio_set_output_options(USART1_TX_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_400KHZ, USART1_TX_PIN);

//	gpio_mode_setup(USART1_TX_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, USART1_TX_PIN);

	gpio_set_af(USART1_TX_PORT, GPIO_AF7, USART1_TX_PIN | USART1_RX_PIN);
	gpio_set_af(USART2_TX_PORT, GPIO_AF7, USART2_TX_PIN | USART2_RX_PIN);
}

void uart_setup(void)
{
	usart_set_databits(USART2, 8);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_baudrate(USART2, 115200);
	usart_enable(USART2);
}

#ifdef SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

int main(int argc, char ** argv)
{
#ifdef SEMIHOSTING
	initialise_monitor_handles();
#endif

	clock_setup();

	gpio_setup();

	uart_setup();

/*	while (1) {
		gpio_set(USART1_TX_PORT, USART1_TX_PIN);
		gpio_clear(USART1_TX_PORT, USART1_TX_PIN);
	}*/

	systick_setup(10);

	os_start();

	return 0;
}
