#include <stdlib.h>
#include <turing.h>

struct walker;

cell *cell_from_bit(int b);
void free_tape(cell *current, cell *previous);
cell *get_next_cell(cell *current, cell *previous);
void link_cells(cell *lef, cell *rit);
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
 * cell_from_bit -- return a new cell marked with bit
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
 * get_next_cell -- return the cell after current via previous
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
 * cell_from_buffer -- read buffer into list of cells
 */
cell *
tape_from_buffer(char *buffer, size_t length)
{
	cell *new_cell;
	cell *tape_tail;
	size_t i;
	int b;

	for (i=0; i/8<length; i+=8) {
		b = bit_at_index(buffer[i/8], i%8);
		new_cell = cell_from_bit(b);
		if (!new_cell) goto fail;

		link_cells(tape_tail, new_cell);
		tape_tail = new_cell;
	}

 fail:
	abort(); // XXX
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
 * walker_begin -- initialize a walker
 */
void
walker_begin(struct walker *walker, cell *current, cell *previous)
{
	walker->current = current;
	walker->previous = previous;
	walker->next = get_next_cell(current, previous);
}

/*
 * walker_step -- step walker one link down the tape
 */
void
walker_step(struct walker *walker)
{
	walker->previous = walker->current;
	walker->current = walker->next;
	walker->next = get_next_cell(walker->current, walker->previous);
}
