#include <stdlib.h>
#include <turing.h>

cell *
cell_from_bit(int b)
{
	cell *result;

	result = malloc(sizeof *result);
	if (!result) return 0;

	*result = b;

	return result;
}
