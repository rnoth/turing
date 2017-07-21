#ifndef _turing_
#define _turing_

enum instr {
	write0 = 0,
	write1 = 1,
	shiftl = 0,
	shiftr = 2,
};

typedef uint16_t action;
typedef uint16_t state;

struct turing;

struct turing *tm_create(size_t nstates);
void tm_destroy(struct turing *tm);
int tm_step(struct turing *tm);
int tm_execute(struct turing *tm);

static inline action trans(state state);

struct turing {
	cell *left;
	cell *right;
	state state;
	action trans[];
};

action
trans(state state)
{
	return state << 2;
}

#endif
