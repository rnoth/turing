#include <string.h>
#include <unit.h>

#include <tmc-parse.c>

#define parse_str(TOK, STR) parse(TOK, STR, strlen(STR))
#define lex_str(TOK, STR) lex(TOK, (uint8_t *)(STR), strlen(STR))

static void ok_token_has_value(struct token *token, char *expected);
static void ok_token_is_type(struct token *token, enum id type);

static void test_lex_id(void);
static void test_lex_id_space(void);

struct unit_test tests[] = {
	{.msg="lexing an identifier",
	 .fun=unit_list(test_lex_id),},
	{.msg="lexing an identifier despite whitespace",
	 .fun=unit_list(test_lex_id_space),},
};

#include <unit.t>

/* avoid typing token names twice */
#define pair(TYPE) [TYPE] = #TYPE
/* we'll probably never have more than 255 token types */
static char *token_types[255] = {
	pair(tok_null),
	pair(tok_eof),
	pair(tok_err),
	pair(tok_colon),
	pair(tok_ignore),
	pair(tok_invert),
	pair(tok_left),
	pair(tok_name),
	pair(tok_pipe),
	pair(tok_right),
	pair(tok_semi),
};

void
ok_token_is_type(struct token *token, enum id expected_type)
{
	if (!token_types[expected_type]) {
		unit_error("unknown value passed to ok_token_is_type\n\t"
		           "I think you added a new token, "
		           "but forgot to add it to token_types[]\n\t"
		           "try adding it now and re-compiling");
	}

	if (token->type == expected_type) return;

	if (!token_types[token->type]) {
		unit_error("unknown value found in token struct "
		           "(inside ok_token_is_type)\n\t"
		           "you may have added a new token, "
		           "and forgot to add it to token_types[]\n\t"
		           "or some buggy code is corrupting the type field");
	}

	unit_fail_fmt("expected type \"%s\" from token, instead of \"%s\"",
	    token_types[expected_type], token_types[token->type]);
	unit_yield();
}

void
ok_token_has_value(struct token *token, char *expected)
{
	uint8_t *expected_uint8=(void*)expected;
	okf(token->value == expected_uint8,
	    "expected \"%s\" from token's value, instead of \"%s\"",
	    expected_uint8, token->value);
}

void
test_lex_id(void)
{
	struct token token[1];
	char *foo="foo";

	try(lex_str(token, foo));
	//ok(token->type == tok_name);
	ok_token_is_type(token, tok_name);
	ok_token_has_value(token, foo);
	expect(3, token->length);
}

void
test_lex_id_space(void)
{
	struct token token[1];
	char *bar="   bar";

	try(lex_str(token, bar));
	ok(token->type == tok_name);
	ok_token_has_value(token, bar + 3);
	expect(3, token->length);
}
