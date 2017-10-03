#ifndef _edna_util_
#define _edna_util_

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#define argv(...) ((char *[]){__VA_ARGS__})
#define arr_len(arr) (sizeof (arr) / sizeof *(arr))
#define write_str(fd, str) write(fd, str, strlen(str))

#define repeat(NTIMES) for (size_t _i=0; _i<NTIMES; ++i)
#define iterate(VAR, NITER) for (size_t VAR=0; VAR<NITER; ++VAR)

#ifndef LONG_BIT
# define LONG_BIT sizeof (long) * 8
#endif

typedef unsigned long ulong;
typedef unsigned int uint;

static inline
bool
in_range(ulong beg, ulong ext, ulong arg)
{
	return arg - beg < ext;
}

static inline
void
lrotate(ulong *lef, ulong *mid, ulong *rit)
{
	ulong tmp = *lef;
	*lef = *mid;
	*mid = *rit;
	*rit = tmp;
}

static inline
void
lshift(ulong *lef, ulong *mid, ulong rit)
{
	*lef = *mid;
	*mid = rit;
}

static inline
void
lshift_ptr(void *lefp, void *midp, void *rit)
{
	*(void **)lefp = *(void **)midp;
	*(void **)midp = rit;
}

static inline
void *
memswp(void *A, void *B, size_t n)
{
	char *a=A, *b=B;
	char m;

	while (n --> 0) {
		m = a[n];
		a[n] = b[n];
		b[n] = m;
	}

	return a;
}

static inline
size_t
next_line(char *buffer, size_t length)
{
	char *nl;

	nl = memchr(buffer, '\n', length);
	if (!nl) return length;
	return nl - buffer + 1;
}

static inline
void
ptr_swap(void *va, void *vb)
{
	void **a=va, **b=vb;
	void *tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static inline
int
ucmp(ulong a, ulong b)
{
	return (a >= b) - (a <= b);
}

static inline
ulong
uclz(ulong a)
{
	if (!a) return LONG_BIT;
	return __builtin_clzl(a);
}

static inline
ulong
ufls(ulong a)
{
	return LONG_BIT - uclz(a);
}

static inline
ulong
umax(ulong a, ulong b)
{
	return a > b ? a : b;
}

static inline
ulong
umin(ulong a, ulong b)
{
	return a < b ? a : b;
}

#endif
