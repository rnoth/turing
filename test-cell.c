#include <unit.h>
#include <cell.c>

static void test_cell_alloc();
static void test_cell_link();
static void test_cell_traverse();
static void test_tape_buffer();
static void test_tape_free();
static void test_tape_read();

struct unit_test tests[] = {
	{.msg = "should allocate cells",
	 .fun = unit_list(test_cell_alloc),},
	{.msg = "should link cells",
	 .fun = unit_list(test_cell_link),},
	{.msg = "should traverse cells",
	 .fun = unit_list(test_cell_traverse),},
	{.msg = "should free tapes",
	 .fun = unit_list(test_tape_free),},
	{.msg = "should convert buffers to tapes",
	 .fun = unit_list(test_tape_buffer),},
	{.msg = "should read tapes into buffers",
	 .fun = unit_list(test_tape_read),},
};

#include <unit.t>

#define make_tape(...) _make_tape((cell **[]){__VA_ARGS__, 0});
void
_make_tape(cell ***cells) 
{
	size_t i;

	for (i=0; cells[i]; ++i) {
		ok(*cells[i] = cell_from_bit(0));
		if (i) try(link_cells(*cells[i-1],*cells[i]));
	}
}

void
test_cell_alloc()
{
	cell *p;

	ok(p = cell_from_bit(0));
	ok(*p == 0);
	try(free(p));

	ok(p = cell_from_bit(1));
	ok(*p == 1);
	try(free(p));
}

void
test_cell_link()
{
	cell *a, *b, *c;

	make_tape(&a, &b, &c);

	ok(*a == (cell)b);
	ok(*b^(cell)c == (cell)a);

	ok(*b^(cell)a == (cell)c);
	ok(*c == (cell)b);

	try(free(a));
	try(free(b));
	try(free(c));
}

void
test_cell_traverse()
{
	cell *a, *b, *c;

	make_tape(&a, &b, &c);

	ok(b == get_next_cell(a, 0));
	ok(c == get_next_cell(b, a));
	ok(0 == get_next_cell(c, b));
	ok(0 == get_next_cell(a, b));
	ok(a == get_next_cell(b, c));

	ok(a == walk_tape(c, 0));
	ok(c == walk_tape(a, 0));

	try(free(a));
	try(free(b));
	try(free(c));
}

void
test_tape_buffer()
{
	struct walker walker[1];
	cell *current;
	cell *tape;
	uint8_t b;
	char ch;
	int i;

	ch = 1;
 again:
	ok(tape = tape_from_buffer(&ch, 1));

	try(walker_begin(walker, tape, 0));

	for (i=0; i<8; ++i) {
		b = bit_at_index(ch, i);
		current = walker->current;
		okf(b == (*current & 1),
		    "expected %hhu in cell %d, got %lu",
		    b, i, *current & 1);
		try(walker_step(walker));
	}

	ok(walker->next == 0x0);

	free_tape(tape, 0);

	ch <<= 1;
	if (ch) goto again;
}

void
test_tape_free()
{
	cell *a, *b, *c;

	make_tape(&a, &b, &c);
	try(free_tape(a, 0));
}

void
test_tape_read()
{
	char string[6]="hello";
	char buffer[6]={0};
	cell *tape;

	ok(tape = tape_from_buffer(string, strlen(string)));
	try(copy_tape_into_buffer(buffer, strlen(string), tape));

	okf(!strcmp(buffer, string),
	    "expected \"%s\", got \"%s\"",
	    string, buffer);
}
