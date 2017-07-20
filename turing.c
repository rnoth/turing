#include <stdlib.h>
#include <turing.h>

cell *cell_from_bit(int b);
void free_tape(cell *current, cell *previous);
cell *get_next_cell(cell *current, cell *previous);
void link_cells(cell *lef, cell *rit);

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

/*
 * free_tape -- free entire tape, starting from anywhere
 */
void
free_tape(cell *current, cell *previous)
{
}

/*
 * get_next_cell -- return the cell after current via previous
 */
cell *
get_next_cell(cell *current, cell *previous)
{
	cell result;

	result = *current;
	result &= ~1;
	result ^= (cell)previous;

	return (cell *)result;
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
