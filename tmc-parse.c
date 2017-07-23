#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <tmc-parse.h>
#include <tmc-state.h>

enum id {
	tok_null,
	tok_eof,
	tok_err,
	tok_colon,
	tok_ignore,
	tok_invert,
	tok_left,
	tok_name,
	tok_pipe,
	tok_right,
	tok_semi,
};

enum context {
	st_error,
	st_name,
	st_write,
	st_shift,
	st_chld,
};

struct token {
	enum id type;
	uint8_t *value;
	size_t length;
};

/* eat_ident - get the offset of the last character in an identifier */
static size_t eat_ident(uint8_t *buffer, size_t length);
/* eat_spaces - get the offset of the first non-space, non-comment character */
static size_t eat_spaces(uint8_t *buffer, size_t length);
/* lex - store the next token from buffer in token */
static size_t lex(struct token *token, uint8_t *buffer, size_t length);

size_t
eat_ident(uint8_t *buffer, size_t length)
{
	size_t offset=0;
	uint8_t *comment;

	if (!length) return 0;

	comment = memchr(buffer, '#', length);
	if (comment) length = comment - buffer;

	while (!isspace(buffer[offset])) {
		if (++offset >= length) {
			return offset;
		}
	}

	return offset;
}

size_t
eat_spaces(uint8_t *buffer, size_t length)
{
	size_t offset=0;
	uint8_t *nl;

	if (!length) return 0;

 again:
	while (isspace(buffer[offset])) {
		if (++offset >= length) {
			return offset;
		}
	}

	if (buffer[offset] == '#') {
		nl = memchr(buffer+offset, '\n', length);
		if (!nl) return length;
		offset = nl - buffer;
		goto again;
	}

	return offset;
}

size_t
lex(struct token *token, uint8_t *buffer, size_t length)
{
	static enum id table[256] = {
		[':'] = tok_colon,
		['|'] = tok_pipe,
		[';'] = tok_semi,
		['>'] = tok_left,
		['<'] = tok_right,
		['^'] = tok_invert,
		['_'] = tok_ignore,
	};
	size_t offset;

	*token = (struct token){0};

	offset = eat_spaces(buffer, length);

	if (offset == length) {
		token->type = tok_eof;
		return offset;
	}

	token->value = buffer;

	token->type = table[buffer[offset]];
	if (token->type) {
		token->length = 1;
		return offset+1;
	}

	token->type = tok_name;
	token->length = offset += eat_ident(buffer, length);
	return offset+1;
}
	
void
parse(struct parse *result, uint8_t *buffer, size_t length)
{
	struct token token[1];
	size_t offset=0;

	*result = (struct parse){0};

	if (!length) return;

	while (!result->error) {
		offset += lex(token, buffer, length);
		if (token->type == tok_eof) return;
	}

	return;
}

void
parse_error(struct parse *result, char *msg)
{
	return;
}
