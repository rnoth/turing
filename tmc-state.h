#ifndef _tmc_state_
#define _tmc_state_

#include <stddef.h>

struct state {
	uint8_t *name; 
	size_t length;
	struct state *chld[2];
	struct state *next;
};

#endif
