#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cell.h>
#include <turing.h>
#include <util.h>

/* execute_op: execute a single intruction
 *
 * We call shift_tape here, which can allocate memory.  So this
 * function can sometimes fail when out of memory.
 */
static int execute_op(struct turing *tm, enum instr op);

/* shift_tape: store the pointer to the next cell in current
 *
 * After calling this function, *current points to the next cell
 * relative to previous.
 *
 * Note that by calling this function with .tape[0] and .tape[1] in
 * different orders is sufficient to move in different directions.
 * See cell.h for details.
 *
 * This function will try to allocate a new cell if we reach either
 * end of the tape. The error code ENOMEM is returned if this
 * allocation fails
 */
static cell *shift_tape(cell *current, cell *previous);

int
execute_op(struct turing *m, enum instr o)
{
	cell *x;
	int b;

	if ((o & 1) == invert) invert_cell_bit(m->tape[0]);

	b = (o & 2) == shiftr;
	x = shift_tape(m->tape[b], m->tape[!b]);
	if (!x) return ENOMEM;
	ptrshift(m->tape + !b, m->tape + b, x);

	return 0;
}

cell *
shift_tape(cell *current_cell, cell *previous_cell)
{
	cell *next_cell;

	next_cell = get_next_cell(current_cell, previous_cell);
	if (next_cell) return next_cell;

	next_cell = cell_from_bit(0);
	if (!next_cell) return 0x0;

	link_cells(next_cell, current_cell);
	return next_cell;
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

	tm->tape[0] = cell_from_bit(0);
	tm->tape[1] = cell_from_bit(0);
	if (!tm->tape[0] || !tm->tape[0]) goto fail;
	link_cells(tm->tape[0], tm->tape[1]);

	memset(tm->delta, 0xff, delta_size);
	tm->state = 0;

	return tm;

 fail:
	free(tm->tape[0]), free(tm->tape[1]);
	free(tm);

	return 0;
}

void
tm_destroy(struct turing *tm)
{
	if (!tm) return;
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
		err = tm_single_step(tm);
		if (err) return err;
	}

	return 0;
}

void
tm_read_symbols(struct turing *tm, void *buffer, size_t length)
{
	cell *init_cell;

	if (!buffer) return;
	if (!length) return;

	if (!tm) {
		memset(buffer, 0, length);
		return;
	}

	init_cell = walk_tape(tm->tape[0], tm->tape[1]);
	copy_tape_into_buffer(buffer, length, init_cell);
}

int
tm_single_step(struct turing *tm)
{
	enum instr op;
	action act;
	state next;
	bool b;
	int err;

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
