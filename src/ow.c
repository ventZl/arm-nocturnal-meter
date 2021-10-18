#include <cmrx/application.h>
#include <cmrx/interface.h>
#include "onewire.h"
#include "ow.h"

#define CLASS struct OneWireBus

int search(SELF, uint8_t * id)
{
	self->search_diff = ow_rom_search(self->search_diff, id);
	return self->search_diff == OW_LAST_DEVICE;
}

void reset(SELF)
{
	ow_reset();
}

void command(SELF, uint8_t command, const uint8_t * id)
{
	return ow_command(command, (uint8_t *) id);
}

void write(SELF, uint8_t byte)
{
	ow_byte_wr(byte);
}

uint8_t read(SELF)
{
	return ow_byte_rd();
}

static VTABLE struct OneWireBusVMT ow_vmt = {
	&search,
	&reset,
	&command,
	&write,
	&read
};

struct OneWireBus onewire = {
	&ow_vmt,
	OW_SEARCH_FIRST
};

#undef CLASS

OS_APPLICATION_MMIO_RANGE(onewire, 0, 0);

OS_APPLICATION(onewire);
