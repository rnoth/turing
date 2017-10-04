#ifndef _turing_str_
#define _turing_str_

/* eat_ident - get the offset of the last non-space character in buffer */
size_t eat_ident(void *buffer, size_t length);
/* eat_spaces - get the offset of the first non-space, non-commented character */
size_t eat_spaces(void *buffer, size_t length);

#endif
