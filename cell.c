#include <stdlib.h>
#include <string.h>

#include <cell.h>

/*
 * struct walker is a little bit of abstraction to avoid
 * duplicating the list walking logic everywhere.
 */
struct walker;

int bit_at_index(char byte, int index);
cell *cell_from_bit(int b);
void copy_tape_into_buffer(char *buffer, size_t length, cell *tape);
void free_subtape(cell *current, cell *previous);
void free_tape(cell *current, cell *previous);
cell *get_next_cell(cell *current, cell *previous);
void link_cells(cell *lef, cell *rit);
cell *tape_from_buffer(char *buffer, size_t length);
cell *walk_tape(cell *current, cell *previous);
void walker_begin(struct walker *walker, cell *current, cell *previous);
void walker_step(struct walker *walker);

struct walker {
	cell *previous;
	cell *current;
	cell *next;
};

/*
 * byte_index -- return the bit at index in byte
 */
int
bit_at_index(char byte, int index)
{
	return !!(byte & 1 << index);
}

/*
 * cell_from_bit -- return a new cell tagged with a bit
 */
cell *
cell_from_bit(int b)
{
	cell *result;

	result = malloc(sizeof *result);
	if (!result) return 0;

	*result = b;

	return result;
}

/*
 * copy_tape_into_buffer -- write the cells of tape into buffer
 *
 * tape must be the first or last cell in a tape
 */
void
copy_tape_into_buffer(char *buffer, size_t length, cell *tape)
{
	struct walker walker[1];
	size_t i;
	int b;

	memset(buffer, 0, length);
	walker_begin(walker, tape, 0);

	i = length * 8;
	while (i --> 0) {
		b = *walker->current & 1;
		buffer[i/8] |= b << i%8;
		walker_step(walker);
	}
}

/*
 * free_subtape -- free all cells after current
 *
 * previous is not freed.
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
 * free_tape -- free entire tape, starting from anywhere
 */
void
free_tape(cell *current, cell *previous)
{
	free_subtape(current, previous);
	free_subtape(previous, current);
}

/*
 * get_next_cell -- return the cell after current
 */
cell *
get_next_cell(cell *current, cell *previous)
{
	cell result;

	if (!current) return 0;

	result = *current;
	result &= ~1;
	result ^= (cell)previous;

	return (cell *)result;
}

/*
 * link_cells -- link two cells
 *
 * Never call this function on cells with two links, it will corrupt the list
 */
void
link_cells(cell *lef, cell *rit)
{
	*lef ^= (cell)rit;
	*rit ^= (cell)lef;
}

/*
 * tape_from_buffer -- read buffer onto a tape
 */
cell *
tape_from_buffer(char *buffer, size_t length)
{
	cell *tape_head=0;
	cell *new_cell;
	size_t i;
	int b;

	for (i=0; i/8<length; ++i) {
		b = bit_at_index(buffer[i/8], i%8);
		new_cell = cell_from_bit(b);
		if (!new_cell) goto fail;

		if (tape_head) link_cells(new_cell, tape_head);
		tape_head = new_cell;
	}

	return tape_head;

 fail:
	free_tape(tape_head, 0);
	return 0;
}

/*
 * walk_tape -- return the end of the tape
 */
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

/*
 * walker_begin -- initialize walker
 */
void
walker_begin(struct walker *walker, cell *current, cell *previous)
{
	walker->current = current;
	walker->previous = previous;
	walker->next = get_next_cell(current, previous);
}

/*
 * walker_step -- step walker one link through the tape
 */
void
walker_step(struct walker *walker)
{
	walker->previous = walker->current;
	walker->current = walker->next;
	walker->next = get_next_cell(walker->current, walker->previous);
}
