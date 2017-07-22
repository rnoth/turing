#ifndef _turing_
#define _turing_
#include <stddef.h>
#include <stdint.h>
#include <cell.h>

/*
 * Instructions are a bitmask.
 *
 * The lowest bit decides if to write the current cell. 0 means let it be, 1
 * means invert it. This simplifies code generation. Otherwise, halting
 * transitions on 1 cells would need an extra bit of attention to not clobber
 * the cell.
 *
 * The second lowest bit is the direction to shift the tape in. Left is 0,
 * right is 1.
 */
enum instr {
	ignore = 0, // 00
	invert = 1, // 01
	shiftl = 0, // 00
	shiftr = 2, // 10
};

/*
 * halt is the highest state number possible, is treated specially: whenever
 * a machine enters it, execution stops. If a machine is already in it,
 * tm_step & tm_execute return an error (-1).
 */

enum halt {
	halt = 0x3fff,
};

/*
 * a state is an index into tm->trans.
 * An action is a state, left-shifted twice. The low 2 bits are an instruction.
 */
typedef uint16_t action;
typedef uint16_t state;

struct turing;

/* tm_create - allocate turing machine with space for 'nstates' transitions */
struct turing *tm_create(size_t nstates);
/* tm_destroy - free turing machine */
void tm_destroy(struct turing *tm);
/* tm_read_symbols - read the tape of tm into a buffer */
void tm_read_symbols(struct turing *tm, void *buffer, size_t length);
/* tm_step - execute one state transition on tm */
int tm_step(struct turing *tm);
/* tm_execute - execute tm's program until it halts */
int tm_execute(struct turing *tm);

/* create a transition */
static inline action arrow(state state, enum instr shift, enum instr write);

/*
 * The Turing machine itself.
 *
 * .tape points to the current cell. Two pointers are necessary to traverse
 * the tape, this is the only use of .tape[1];
 *
 * .state is the current state.
 *
 * .trans is a table with two actions for every state, determined from the
 * symbol in the current cell.
 */
struct turing {
	cell *tape[2];
	state state;
	action delta[][2];
};

action
arrow(state state, enum instr shift, enum instr write)
{
	return state << 2 | shift | write;
}

#endif
