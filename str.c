#include <ctype.h>
#include <stddef.h>
#include <string.h>

size_t
eat_ident(char *buffer, size_t length)
{
	size_t offset=0;
	char *comment;

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
eat_spaces(char *buffer, size_t length)
{
	size_t offset=0;
	char *nl;

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
