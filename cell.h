#ifndef _turing_cell_
#define _turing_cell_
#include <stdbool.h>
#include <stdint.h>

/*
 * A Turing machine's tape is represented as a xor-linked list.
 *
 * There are two symbols, and these symbols are tagged into the
 * link at the lowest bit, viz.
 *   cell |=  1; // mark cell with the symbol '1'
 *   cell &= ~1; // mark cell with the symbol '0'
 *
 * This is symbol is called variously the 'bit', 'symbol', 'mark', etc.
 * Changing the symbol is called variously 'marking', 'writing', etc.
 *
 * Because cells make a xor-linked list, you must carry around a current &
 * previous pointer to traverse it. To follow a link, you mask out the
 * symbol, and xor it with the previous cell's *address* (not it's contents);
 *
 * In symbols:
 *
 *   cell *result, temp;
 *   temp = *current;
 *   temp &= ~1;             // mask out symbol
 *   temp ^= (cell)previous;
 *   result = (cell *)temp;
 *
 * 'result' now holds the next cell in the tape.
 */
typedef uintptr_t cell;

/*
 * struct walker is a little bit of abstraction to avoid
 * duplicating the list walking logic everywhere.
 *
 * Use it by calling walker_begin() with your current & previous pointers,
 * and then call walker_step() in a loop.
 *
 * When walker->current becomes the null pointer, there are no more cells.
 * When walker->next becomes the null pointer, current is the last cell.
 */
struct walker;

/* bit_of_cell: read the bit of the given cell */
static inline int bit_of_cell(cell *cell);
/* invert_cell_bit: flip the bit inside the given cell */
static inline void invert_cell_bit(cell *cell);

/* cell_from_bit - return a new cell tagged with a 'bit' */
cell *cell_from_bit(bool bit);
/* copy_tape_into_buffer - copy symbols of tape to a buffer 'length' bytes long
 * The 'tape' *must* be the first or last cell in it's tape */
void copy_tape_into_buffer(char *buffer, size_t length, cell *tape);
/* free_tape - free tape, starting from anywhere */
void free_tape(cell *current, cell *previous);
/* get_next_cell - get cell after current, from previous */
cell *get_next_cell(cell *current, cell *previous);
/* link cells - link two cells
 * Never call this function on cells with two links, it will corrupt the list */
void link_cells(cell *left, cell *right);
/* tape_from_buffer - read buffer onto tape */
cell *tape_from_buffer(char *buffer, size_t length);
/* walk_tape - walk to (and return) the end of a tape */
cell *walk_tape(cell *current, cell *previous);
/* walker_begin - initial a walker */
void walker_begin(struct walker *walker, cell *current, cell *previous);
/* walker_step - step walker one cell down the tape */
void walker_step(struct walker *walker);

struct walker {
	cell *previous;
	cell *current;
	cell *next;
};

int
bit_of_cell(cell *cell)
{
	return *cell & 1;
}

void
invert_cell_bit(cell *cell)
{
	*cell ^= 1;
}

#endif
