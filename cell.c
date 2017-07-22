#include <stdlib.h>
#include <string.h>

#include <cell.h>

static int bit_at_index(char byte, int index);
static void free_subtape(cell *current, cell *previous);

/*
 * We assume index is between 0 & 7, and create a bit mask to extract only that
 * bit from byte. Finally, we double negate it to convert it to 0 or 1, because
 * our bitmask extracts bits as powers of two.
 */
int
bit_at_index(char byte, int index)
{
	return !!(byte & 1 << index);
}

/*
 * Because a new cell has no links (i.e., they're null), the cell we return is
 * blank except for the symbol in the cell, 0 or 1. As boolean 'true' is 1,
 * we can just write it into the cell.
 */
cell *
cell_from_bit(bool b)
{
	cell *result;

	result = malloc(sizeof *result);
	if (!result) return 0;

	*result = b;

	return result;
}

/*
 * Because it's simplest to iterate over bits in our tape than over bytes in
 * our buffer, we will write out bits one by one, which requires zeroing the
 * bytes before we first write to them. We have chosen to do this upfront with
 * memset rather than having the logic inside the main loop.
 */
void
copy_tape_into_buffer(char *buffer, size_t length, cell *tape)
{
	struct walker walker[1];
	size_t i;
	int b;

	memset(buffer, 0, length);
	walker_begin(walker, tape, 0);

	for (i=0; i/8<length; ++i) {
		b = *walker->current & 1;
		buffer[i/8] |= b << i%8;
		walker_step(walker);
		if (!walker->current) return;
	}
}

/*
 * Because free_tape() needs to free in both directions, this function handles
 * freeing in a particular direction. Two pointers specify a direction, and
 * this function only frees one of them. See free_tape() itself for quick
 * justification.
 */
void
free_subtape(cell *current, cell *previous)
{
	struct walker walker[1];

	walker_begin(walker, current, previous);

	while (walker->current) {
		free(walker->current);
		walker_step(walker);
	}
}

/*
 * For convenience, this function will free an entire tape, no matter where
 * the right and left pointers indicate. Because of this, it defers to
 * free_subtape(), which left & right taking turns as the current & previous
 * pointers.
 */
void
free_tape(cell *left, cell *right)
{
	free_subtape(left, right);
	free_subtape(right, left);
}

cell *
get_next_cell(cell *current, cell *previous)
{
	cell temp;

	if (!current) return 0;

	temp = *current;
	temp &= ~1;
	temp ^= (cell)previous;

	return (cell *)temp;
}

void
link_cells(cell *lef, cell *rit)
{
	*lef ^= (cell)rit;
	*rit ^= (cell)lef;
}

/*
 * Tapes are stored in little endian fashion. This is also how bytes are
 * addressed, so i%8 goes from lowest bit to highest, and i/8 goes from
 * lowest offset to highest. Thus, 'b' becomes every bit from buffer in
 * order.
 */
cell *
tape_from_buffer(char *buffer, size_t length)
{
	cell *tape_head=0;
	cell *tape_tail;
	cell *new_cell;
	size_t i;
	int b;

	for (i=0; i/8<length; ++i) {
		b = bit_at_index(buffer[i/8], i%8);

		new_cell = cell_from_bit(b);
		if (!new_cell) goto fail;

		if (tape_head) link_cells(tape_tail, new_cell);
		else tape_head = new_cell;

		tape_tail = new_cell;
	}

	return tape_head;

 fail:
	free_tape(tape_head, 0);
	return 0;
}

/* TODO: use a walker here */
cell *
walk_tape(cell *current, cell *previous)
{
	cell *next;

	while (next = get_next_cell(current, previous)) {
		previous = current;
		current = next;
	}
	return current;
}

void
walker_begin(struct walker *walker, cell *current, cell *previous)
{
	walker->current = current;
	walker->previous = previous;
	walker->next = get_next_cell(current, previous);
}

void
walker_step(struct walker *walker)
{
	walker->previous = walker->current;
	walker->current = walker->next;
	walker->next = get_next_cell(walker->current, walker->previous);
}
