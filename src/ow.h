#pragma once

#include <cmrx/interface.h>

struct OneWireDevice;

struct OneWireBusVMT {
	int (*search)(SELF, uint8_t * id);
	void (*reset)(SELF);
	void (*command)(SELF, uint8_t command, const uint8_t * id);
	void (*write)(SELF, uint8_t byte);
	uint8_t (*read)(SELF);
};

struct OneWireBus {
	const struct OneWireBusVMT * vtable;
	uint8_t search_diff;
};

extern struct OneWireBus onewire;
