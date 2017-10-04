#include <unit.h>
#include <str.c>

static void test_space(void);
static void test_ident(void);
static void test_ident_empty(void);

struct unit_test tests[] = {
	{.msg="getting the length of non-space characters in a buffer",
	 .fun=unit_list(test_ident),},
	{.msg="getting the length of an empty identifier",
	 .fun=unit_list(test_ident_empty),},
	{.msg="getting the length of a sequence of spaces",
	 .fun=unit_list(test_space),},
};

#include <unit.t>

void
test_ident(void)
{
	char *hii="hii";

	expect(3, eat_ident(hii, strlen(hii)));
}

void
test_ident_empty(void)
{
	char *ws="  \n\t";

	expect(0, eat_ident(ws, strlen(ws)));
}

void
test_space(void)
{
	char *ws="  \nstop";

	expect(3, eat_spaces(ws, strlen(ws)));
}
