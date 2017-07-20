#include <unit.h>
#include <turing.c>

static void test_cell_alloc();
static void test_cell_link();
static void test_free_tape();

struct unit_test tests[] = {
	{.msg = "should allocate cells",
	 .fun = unit_list(test_cell_alloc),},
	{.msg = "should link cells",
	 .fun = unit_list(test_cell_link),},
	{.msg = "should be able to free tapes",
	 .fun = unit_list(test_free_tape),},
};

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
test_free_tape()
{
	cell *a, *b, *c;

	make_tape(&a, &b, &c);
	try(free_tape(a, 0));
}

int
main(int argc, char **argv)
{
	unit_parse_args(argv);
	return unit_run_tests(tests, sizeof tests/sizeof *tests);
}
