#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <cell.h>
#include <turing.h>

static int execute_op(struct turing *tm, enum instr op);
static action transition(action *table, state state, bool b);
static int shift(cell **current, cell **previous);

int
execute_op(struct turing *tm, enum instr op)
{
	if (op & write1) {
		*tm->left |= 1;
	} else {
		*tm->left &= ~1;
	}

	if (op & shiftl) {
		return shift(&tm->left, &tm->right);
	} else {
		return shift(&tm->right, &tm->left);
	}
}

action
transition(action *table, state state, bool b)
{
	return table[state*2+b];
}

int
shift(cell **current, cell **previous)
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

	link_cells(*current, next);

	*previous = *current;
	*current = next;

	return 0;
}

struct turing *
tm_create(size_t nstates)
{
	struct turing *tm;
	size_t mem;

	mem = sizeof *tm + nstates*sizeof *tm->trans;

	return calloc(mem, 1);
}

void
tm_destroy(struct turing *tm)
{
	free_tape(tm->left, tm->right);
	free(tm);
}

int
tm_execute(struct turing *tm)
{
	int err;
	while (tm->state) {
		err = tm_step(tm);
		if (err) return err;
	}

	return 0;
}

int
tm_step(struct turing *tm)
{
	enum instr op;
	action act;
	state next;
	int err;
	bool b;

	b = *tm->left & 1;
	act = transition(tm->trans, tm->state, b);

	op = act & 3;
	next = act >> 2;

	err = execute_op(tm, op);
	if (err) return err;

	tm->state = next;

	return 0;
}
