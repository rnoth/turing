#include <unit.h>

#include <turing.c>

void test_alloc();
void test_extend();
void test_shift();
void test_trivial();

struct unit_test tests[] = {
	{.msg = "allocating & freeing turing machines",
	 .fun = unit_list(test_alloc),},
	{.msg = "shifting the tape",
	 .fun = unit_list(test_shift),},
	{.msg = "executing a trivial turing machine",
	 .fun = unit_list(test_trivial),},
	{.msg = "extending the tape as needed",
	 .fun = unit_list(test_extend),},
};

#include <unit.t>

void
test_alloc()
{
	struct turing *tm;

	ok(tm = tm_create(10));
	try(tm_destroy(tm));
}

void
test_extend()
{
	struct turing *tm;
	uint8_t buffer[1];
	int i;

	tm = tm_create(8);

	for (i=0; i<8; ++i) {
		tm->delta[i][0] = arrow(i+1, shiftr, invert);
	}

	tm->delta[8][0] = arrow(halt, shiftl, invert);

	ok(!tm_execute(tm));

	try(tm_read_symbols(tm, buffer, 1));
	okf(*buffer == 0xff,
	    "expected to see 0xff on the tape, got 0x%hhx",
	    *buffer);
}

void
test_shift()
{
	cell *a, *b, *c;

	a = cell_from_bit(0), b = cell_from_bit(1);

	link_cells(a, b);

	ok(c = shift_tape(b, a));

	ok(!bit_of_cell(c));
	ok( bit_of_cell(b));
	ok(!bit_of_cell(a));

	ok(get_next_cell(b, a) == c);
	ok(get_next_cell(a, b) == 0);

	ok(shift_tape(b, a) == c);

	try(free_tape(a, b));
}

void
test_trivial()
{
	struct turing *tm;

	tm = tm_create(1);

	ok(!tm_single_step(tm));
	okf(tm->state == halt,
	    "expected machine to be in state 0x%x, instead of 0x%hx",
	    halt, tm->state);
	ok(tm_single_step(tm) == -1);

	try(tm_destroy(tm));
}
