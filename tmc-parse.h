#ifndef _tmc_parse_
#define _tmc_parse_

#include <stdbool.h>
#include <stdint.h>

struct parse;
struct syntax;

struct parse {
	int error;
	size_t line;
	size_t byte;
	struct state *list;
};

void parse(struct parse *result, uint8_t *buffer, size_t length);

#endif
