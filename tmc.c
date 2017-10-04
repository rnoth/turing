#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include <turing.h>
#include <tmc-parse.h>

static int run(char **argv);

int
run(char **argv)
{
	struct parse ir[1]={0};
	char *fn = *argv;
	uint8_t *map;
	off_t len;
	int fd;

	fd = open(fn, O_RDONLY);
	if (fd == -1) perror("open failed"), exit(EX_NOINPUT);

	len = lseek(fd, SEEK_END, 0);
	if (fd == -1) perror("lseek failed"), exit(EX_OSERR);
	
	map = mmap(0x0, len, PROT_READ, MAP_PRIVATE, fd, 0);

	parse(ir, map, len);

	if (ir->error) {
		fprintf(stderr, "%s", ir->message);
		goto finally;
	}

 finally:
	munmap(map, len);
	close(fd);

	return ir->error;
}

void
usage(void)
{
	fprintf(stderr, "usage: tmc <file>\n");
}

int
main(int argc, char **argv)
{
	if (argc < 2) usage(), exit(EX_USAGE);

	return run(argv+1);
}
