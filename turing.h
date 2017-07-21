#ifndef _turing_
#define _turing_
#include <stdint.h>

/*
 * A Turing machine's tape is represented as a xor-linked list.
 * There are two symbols, and these symbols are tagged into the
 * link.
 */
typedef uintptr_t cell;

typedef uint16_t state;
typedef uint16_t action;

struct turing {
	cell  *current;
	cell  *previous;
	state  state;
	action trans[];
};
#endif
