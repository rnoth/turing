#include <string.h>
#include <unit.h>

#include <tmc-parse.c>

#define parse_str(res, str) parse(res, str, strlen(str))

static void test_lex_id(void);
static void test_lex_id_space(void);

struct unit_test tests[] = {
	{.msg="lexing an identifier",
	 .fun=unit_list(test_lex_id),},
	{.msg="lexing an identifier despite whitespace",
	 .fun=unit_list(test_lex_id_space),},
};

#include <unit.t>

void
test_lex_id(void)
{
	void *foo=(void *)"foo";
	struct token tok[1];

	try(lex(tok, foo, strlen(foo)));
	ok(tok->type == tok_name);
	ok(tok->value == foo);
	expect(3, tok->length);
}

void
test_lex_id_space(void)
{
	uint8_t *bar=(uint8_t*)"   bar";
	struct token tok[1];

	try(lex(tok, bar, 3));
	ok(tok->type == tok_name);
	ok(tok->value == (uint8_t*)bar + 3);
	expect(3, tok->length);
}
