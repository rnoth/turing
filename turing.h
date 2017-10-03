#ifndef _turing_
#define _turing_
#include <stddef.h>
#include <stdint.h>
#include <cell.h>

/*
 * Instructions are a bitmask.
 *
 * The lowest bit determines whether to write the current cell.
 * 
 * 0 means do not change, and 1 means invert the bit in the cell.
 *
 * This somewhat simplifies machine initialisation.  With this scheme,
 * filling the array with set bits creates a valid machine.  This is
 * the machine which halts on each transition, but without clobbering
 * the bit inside the current cell.
 *
 * We consider this a sane defaut.  The alternative to this is having
 * the low bit determine which bit to write.  Here, every transition
 * would write some bit into the cell.  However, if the bit just read
 * is written out, this is the same as not writing a bit at all.
 *
 * The issue with this scheme is our simplistic initialisation
 * strategy of filling the transition array with zeros would, in this
 * scheme, unconditionally set the bit in the current cell.  It is
 * convenient for client programs to assume that every transition they
 * have no manipulated is a simple halting transition.  It is also
 * convenient for there to be single, static value which creates these
 * pure halting transition.
 *
 * Overall, we feel this is the simplest solution.
 *
 * The second lowest bit in the mask is the direction for the tape
 * head to move on this transition.  0 causes the tape to move left, 2
 * moves it right.
 * 
 */
enum instr {
	invert = 0,
	ignore = 1,
	shiftl = 0,
	shiftr = 2,
};

/*
 * "halt" is the highest state number possible, is treated specially:
 * whenever a machine enters it, execution stops.  If a machine is
 * already in it, tm_step & tm_execute will return an error code (-1).
 */

enum halt {
	halt = 0x3fff,
};

/*
 * A state is an index into tm->trans.
 * 
 * An action is a state, left-shifted twice.  The low 2 bits are an
 * instruction.
 */
typedef uint16_t action;
typedef uint16_t state;

struct turing;

/* tm_create: allocate a machine with space for 'nstates' transitions */
struct turing *tm_create(size_t nstates);
/* tm_destroy: free a machine returned by tm_create */
void tm_destroy(struct turing *tm);
/* tm_read_symbols: read the tape of tm into buffer of length bytes */
void tm_read_symbols(struct turing *tm, void *buffer, size_t length);
/* tm_single_step: execute one state transition of tm */
int tm_single_step(struct turing *tm);
/* tm_execute: step through tm's transitions until it halts */
int tm_execute(struct turing *tm);

/* arrow: combine state, shift, and write into a transition */
static inline action arrow(state state, enum instr shift, enum instr write);

/*
 * This is the Turing machine itself.
 *
 * .tape points at the current cell. Tapes are a xor-linked list, and
 * so two pointers are necessary for traversal.  This is the only use
 * of .tape[1];
 *
 * .state is the current state index.
 *
 * .trans is a table with two actions for each state.  These two
 * actions are indexed by the bit in the current cell.  The selected
 * action is callled the transition.
 */
struct turing {
	cell *tape[2];
	state state;
	action delta[][2];
};


/*
 * To create a transition, we must store the side-effects in the lower
 * bits.  The next state is stored in the remaining bits.
 */
action
arrow(state state, enum instr shift, enum instr write)
{
	return state << 2 | shift | write;
}

#endif
