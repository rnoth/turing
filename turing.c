#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cell.h>
#include <turing.h>

/* execute_op - execute a single intruction
 * execute_op calls shift_tape, and thus can fail when out of memory */
static int execute_op(struct turing *tm, enum instr op);
/* shift_tape - moves current into previous, and the next state into current
 * This function will allocate a new cell if necessary. ENOMEM is returned
 * if the allocation fails */
static int shift_tape(cell **current, cell **previous);

int
execute_op(struct turing *tm, enum instr op)
{
	if ((op & 1) == invert) *tm->left ^= !(*tm->left & 1);

	if ((op & 2) == shiftl) {
		return shift_tape(&tm->left, &tm->right);
	} else {
		return shift_tape(&tm->right, &tm->left);
	}
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
	size_t delta_size;

	if (!nstates) return 0;

	delta_size = sizeof tm->delta[0] * nstates;
	tm = calloc(sizeof *tm + delta_size, 1);
	if (!tm) return 0;

	tm->left = cell_from_bit(0);
	if (!tm->left) goto fail;
	tm->right = cell_from_bit(0);
	if (!tm->right) goto fail;

	link_cells(tm->left, tm->right);

	memset(tm->delta, 0xff, delta_size);

	tm->state = 0;

	return tm;

 fail:
	free(tm->left);
	free(tm->right);
	free(tm);

	return 0;
}

void
tm_destroy(struct turing *tm)
{
	if (!tm) return;
	free_tape(tm->left, tm->right);
	free(tm);
}

int
tm_execute(struct turing *tm)
{
	int err;

	if (!tm) return EFAULT;
	if (tm->state == halt) return -1;

	while (tm->state != halt) {
		err = tm_single_step(tm);
		if (err) return err;
	}

	return 0;
}

void
tm_read_symbols(struct turing *tm, void *buffer, size_t length)
{
	cell *tape_start;

	if (!tm) return;
	if (!buffer) return;
	if (!length) return;

	tape_start = walk_tape(tm->left, tm->right);
	copy_tape_into_buffer(buffer, length, tape_start);
}

int
tm_single_step(struct turing *tm)
{
	enum instr op;
	action act;
	state next;
	int err;
	bool b;

	if (!tm) return EFAULT;
	if (tm->state == halt) return -1;

	b = *tm->left & 1;
	act = tm->delta[tm->state][b];

	op = act & 3;
	next = act >> 2;

	err = execute_op(tm, op);
	if (err) return err;

	tm->state = next;

	return 0;
}
