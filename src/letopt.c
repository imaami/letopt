/* SPDX-License-Identifier: LGPL-2.1-or-later */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCLUDED_FROM_LETOPT_C_
#include "letopt.h"
#undef INCLUDED_FROM_LETOPT_C_

#ifdef __clang_major__
# if __clang_major__ >= 14
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#  if __clang_major__ >= 16
#   pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#  endif // __clang_major__ >= 16
#  if __clang_major__ == 17
#   pragma clang diagnostic ignored "-Wpre-c2x-compat"
#  endif // __clang_major__ == 17
#  if __clang_major__ >= 18
#   pragma clang diagnostic ignored "-Wpre-c23-compat"
#  endif // __clang_major__ >= 18
#  if __clang_major__ >= 19
#   pragma clang diagnostic ignored "-Wpre-c11-compat"
#  endif // __clang_major__ >= 19
# endif // __clang_major__ >= 14
#endif // __clang_major__

extern_letopt_state_init();
extern_letopt_get_number_arg();
extern_letopt_get_string_arg();
extern_letopt_get_long_opt_arg();

#undef extern_letopt_state_init
#undef extern_letopt_get_number_arg
#undef extern_letopt_get_string_arg
#undef extern_letopt_get_long_opt_arg

struct letopt_state
letopt_state_init (int    argc,
                   char **argv)
{
	struct letopt_state state = {
		.v = argv,
		.c = argc
	};

	if (state.c > 1 &&
	    !(state.q = calloc((size_t)state.c, sizeof *state.q)))
		state.e = errno ? errno : ENOMEM;

	return state;
}

bool
letopt_get_number_arg (struct letopt_state *const state,
                       int64_t *const             dest,
                       const int64_t              min,
                       const int64_t              max)
{
	if (!*state->p) {
		state->e = EINVAL;
		return false;
	}

	errno = 0;

	char *end = state->p;
	int64_t n = _Generic(n
		, long: strtol
		, long long: strtoll
	)(state->p, &end, 0);

	int e = errno;
	if (!e) {
		if (*end) {
			e = EINVAL;
		} else if (n < min || n > max) {
			e = ERANGE;
		} else {
			*dest = n;
			return true;
		}
	}

	state->e = e;
	return false;
}

bool
letopt_get_string_arg (struct letopt_state *const state,
                       char const **const         dest)
{
	if (!*state->p) {
		state->e = EINVAL;
		return false;
	}

	*dest = state->p;
	return true;
}

int
letopt_get_long_opt_arg (struct letopt_state *const state,
                         const size_t               len)
{
	do {
		if (!state->p[len]) {
			if (++state->i >= state->c)
				break;
			state->p = state->v[state->i];
		} else {
			if (state->p[len] != '=')
				return EAGAIN;
			state->p += len + 1U;
		}

		if (*state->p)
			return 0;
	} while(0);

	state->e = EINVAL;
	return EINVAL;
}

#ifdef __clang_major__
# if __clang_major__ >= 14
#  pragma clang diagnostic pop
# endif // __clang_major__ >= 14
#endif // __clang_major__
