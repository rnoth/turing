#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <cell.h>
#include <turing.h>

static int execute_op(struct turing *tm, enum instr op);
static void init_delta(action delta[][2], size_t nstates);
static int init_tape(cell **);
static int shift_tape(cell **current, cell **previous);

int
execute_op(struct turing *tm, enum instr op)
{
	if (op & invert) *tm->tape[0] ^= !(*tm->tape[0] & 1);

	if (op & shiftr) {
		return shift_tape(tm->tape, tm->tape +1);
	} else {
		return shift_tape(tm->tape + 1, tm->tape);
	}
}

void
init_delta(action delta[][2], size_t nstates)
{
	size_t i;
	for (i=0; i<nstates; ++i) {
		delta[i][0] = arrow(halt, shiftl, ignore);
		delta[i][1] = arrow(halt, shiftl, ignore);
	}
}

int
init_tape(cell **tape)
{
	tape[0] = cell_from_bit(0);
	if (!tape[0]) return ENOMEM;

	tape[1] = cell_from_bit(0);
	if (!tape[1]) {
		free(tape[0]);
		return ENOMEM;
	}

	link_cells(tape[0], tape[1]);

	return 0;
}

int
shift_tape(cell **current, cell **previous)
{
	cell *next;

	next = get_next_cell(*current, *previous);
	if (next) {
		*previous = *current;
		*current = next;
		return 0;
	}

	next = cell_from_bit(0);
	if (!next) return ENOMEM;

	link_cells(next, *current);

	*previous = *current;
	*current = next;

	return 0;
}

struct turing *
tm_create(size_t nstates)
{
	struct turing *tm;
	size_t mem;
	int err;

	if (!nstates) return 0;

	mem = sizeof *tm + nstates * sizeof tm->delta[0];
	tm = calloc(mem, 1);
	if (!tm) return 0;

	err = init_tape(tm->tape);
	if (err) goto fail;

	init_delta(tm->delta, nstates);

	return tm;

 fail:
	free_tape(tm->tape[0], tm->tape[1]);
	free(tm);

	return 0;
}

void
tm_destroy(struct turing *tm)
{
	free_tape(tm->tape[0], tm->tape[1]);
	free(tm);
}

int
tm_execute(struct turing *tm)
{
	int err;

	if (!tm) return EFAULT;
	if (tm->state == halt) return -1;

	while (tm->state != halt) {
		err = tm_step(tm);
		if (err) return err;
	}

	return 0;
}

void
tm_read_symbols(struct turing *tm, void *buffer, size_t length)
{
	cell *start;

	start = walk_tape(tm->tape[0], tm->tape[1]);
	copy_tape_into_buffer(buffer, length, start);
}

int
tm_step(struct turing *tm)
{
	enum instr op;
	action act;
	state next;
	int err;
	bool b;

	if (!tm) return EFAULT;
	if (tm->state == halt) return -1;

	b = *tm->tape[0] & 1;
	act = tm->delta[tm->state][b];

	op = act & 3;
	next = act >> 2;

	err = execute_op(tm, op);
	if (err) return err;

	tm->state = next;

	return 0;
}
