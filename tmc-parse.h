#ifndef _tmc_parse_
#define _tmc_parse_

#include <stdbool.h>
#include <stdint.h>

#define msg_len 256
struct parse {
	int error;
	char message[256];
	struct syntax *state_list;
	struct syntax *last_state;
};

void parse(struct parse *result, uint8_t *buffer, size_t length);

#endif
