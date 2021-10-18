#include <cmrx/application.h>
#include <cmrx/ipc/rpc.h>
#include <cmrx/ipc/timer.h>
#include <cmrx/ipc/signal.h>
#include <cmrx/ipc/thread.h>
#include <libopencm3/stm32/usart.h>
#include "onewire.h"
#include "ds18x20.h"
#include <stdio.h>

uint8_t ids[16][OW_ROMCODE_SIZE];
int32_t temp_dc;

char outbuf[255];
char temp_str[10];

uint8_t remap[] = { 2, 0, 1 };

void sputs(const char * str)
{
	int q = 0;
	while (str[q] != 0)
	{
		usart_send_blocking(USART2, str[q]);
		q++;
		if (q == 255) break;
	}
}

int logger_main(void * arg)
{
	ow_init((char *) USART1);
	ow_reset();

	sputs("Temperature metering service started\r\n");

	uint8_t c = 0, diff = OW_SEARCH_FIRST;

	while(diff != OW_LAST_DEVICE) {
		uint8_t * id = ids[c];
		diff = ow_rom_search(diff, id);
		if(diff == OW_ERR_PRESENCE) {
			sputs("All devices are offline now.\r\n");
			ow_finit();
			return 1;
		}
		if(diff == OW_ERR_DATA) {
			sputs("Bus error.\r\n");
			ow_finit();
			return 1;
		}
		sprintf(outbuf, "Device %03u Type 0x%02hx ID %02hx%02hx%02hx%02hx%02hx%02hx CRC 0x%02hx\r\n", \
					 c, id[0], id[6], id[5], id[4], id[3], id[2], id[1], id[7]);
		sputs(outbuf);
		c++;
	}

	sprintf(outbuf, "Found %d devices\r\n", c);
	sputs(outbuf);
	setitimer(10000000);

	while (1) {
		for (int q = 0; q < c; ++q) {
			uint8_t * id = ids[remap[q]];
			if (DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL) == DS18X20_OK) {
				while (DS18X20_conversion_in_progress() == DS18X20_CONVERTING) {
					usleep(10000); /* It will take a while */
				}
				if (DS18X20_read_maxres(id, &temp_dc) == DS18X20_OK) {
					DS18X20_format_from_maxres(temp_dc, temp_str, sizeof(temp_str));
					/* Copied from my MCU code, so no float point */
					sprintf(outbuf, "%d = %s C  ", q, temp_str);
					sputs(outbuf);
					continue;
				}
				else
				{
					sputs("Unable to read device!\r\n");
				}
			}
		}
		sputs("\r\n");
		kill(get_tid(), SIGSTOP);
	}

	sputs("Done!\r\n");

	return 0;
}

OS_APPLICATION_MMIO_RANGE(logger, 0x00000000, 0x60000000);
OS_APPLICATION(logger);
OS_THREAD_CREATE(logger, logger_main, NULL, 64);
