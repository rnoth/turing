#ifndef _turing_cell_
#define _turing_cell_
#include <stdint.h>

/*
 * A Turing machine's tape is represented as a xor-linked list.
 * There are two symbols, and these symbols are tagged into the
 * link.
 */
typedef uintptr_t cell;

/*
 * struct walker is a little bit of abstraction to avoid
 * duplicating the list walking logic everywhere.
 */
struct walker;

cell *cell_from_bit(int);
void copy_tape_into_buffer(char *buffer, size_t length, cell *tape);
void free_tape(cell *current, cell *previous);
cell *get_next_cell(cell *current, cell *previous);
void link_cells(cell *left, cell *right);
cell *tape_from_buffer(char *buffer, size_t length);
cell *walk_tape(cell *current, cell *previous);
void walker_begin(struct walker *walker, cell *current, cell *previous);
void walker_step(struct walker *walker);

struct walker {
	cell *previous;
	cell *current;
	cell *next;
};

#endif