#include <unit.h>

int
main(int argc, char **argv)
{
	unit_parse_args(argv);
	return unit_run_tests(0, 0);
}
