/* SPDX-License-Identifier: BSD-2-Clause */
/* getopt() alternate implimentation */
#define _GNU_SOURCE
#define GETOPT_ALT_C
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "getopt-alt.h"

#define	__force_cast

#define	ElementsOf(array)	((sizeof(array)) / (sizeof((array)[0])))

#if (!defined(__maybe_unused))
#if defined(__GNUC__)
#define __maybe_unused __attribute__((unused))
#else
#define __maybe_unused
#endif /* defined(__GNUC__) */
#endif /* (!defined(__maybe_unused)) */

#if (!defined(INVALID_FD))
#define	INVALID_FD	(-1)
#endif

char	*optarg_alt = NULL;
int	optind_alt = 1;
int	opterr_alt = 1;
int	optopt_alt = GETOPT_ALT_UNKNOWN;

GetOptAlt GetOptAltState = {
	.OptIndex = 0,
	.OptLetterIndex = 0,
	.OptionEnds = false,
};

void GetOptAltClear(GetOptAlt *g)
{	g->OptIndex = 0;
	g->OptLetterIndex = 0;
	g->OptionEnds = false;
}

void GetOptAltInit(GetOptAlt *g)
{	g->OptIndex = 1;
	g->OptLetterIndex = 0;
	g->OptionEnds = false;
}

bool GetOptAltIsCleared(GetOptAlt *g)
{	return (g->OptIndex == 0);
}

bool GetOptAltIsNoMore(GetOptAlt *g, int argc, char * const *argv)
{	char	*p;
	int	index;

	if (g->OptionEnds) {
		return true;
	}

	if (GetOptAltIsCleared(g)) {
		return true;
	}

	index = g->OptIndex;
	if (index >= argc) {
		return true;
	}

	p = argv[index];
	if (p == NULL) {
		return true;
	}

	if (*p != '-') {
		return true;
	}

	return false; /* note: Check OptIndex points argument "--" */
}

int GetOptAltGetOpt(GetOptAlt *g, int argc, char * const *argv, const char *optstring)
{	char		*p;
	char		*parg;
	int		index;
	int		letter;
	int		opt;
	const char	*find_opt;

	index = g->OptIndex;

	if (GetOptAltIsCleared(g)) {
		/* 1st call. */
#if (defined(GETOPT_ALT_DEBUG))
		fprintf(stderr, "%s: DEBUG: 1st call.\n", __func__);
#endif /* (defined(GETOPT_ALT_DEBUG)) */
		index = 1;
		GetOptAltInit(g);
	} else {
		int	i;

		i = optind_alt;
		if (index != optind_alt) {
			/* Requested restarting parse at optind_alt */
			if (i <= 1) {
				i = 1;
			}
			if (i >= argc) {
				i = argc;
			}
			g->OptIndex = index = i;
			g->OptLetterIndex = 0;
			g->OptionEnds = false;
		}
	}

	if (GetOptAltIsNoMore(g, argc, argv)) {
#if (defined(GETOPT_ALT_DEBUG))
		fprintf(stderr, "%s: DEBUG: No more option. index=%d\n",
			__func__, index
		);
#endif /* (defined(GETOPT_ALT_DEBUG)) */
		g->OptIndex = index;
		g->OptLetterIndex = 0;
		g->OptionEnds = true;
		optarg_alt = NULL;
		optind_alt = index;
		optopt_alt = GETOPT_ALT_NOMORE;
		return GETOPT_ALT_NOMORE;
	}

#if (defined(GETOPT_ALT_DEBUG))
		fprintf(stderr, "%s: DEBUG: Parse option start. index=%d\n",
			__func__, index
		);
#endif /* (defined(GETOPT_ALT_DEBUG)) */

	while (((p = argv[index]) != NULL) && (*p == '-')) {
		letter = g->OptLetterIndex;

#if (defined(GETOPT_ALT_DEBUG))
		fprintf(stderr, "%s: DEBUG: Parse option loop. index=%d, letter=%d\n",
			__func__, index, letter
		);
#endif /* (defined(GETOPT_ALT_DEBUG)) */

		if (letter == 0) {
			if (strcmp(p, "-") == 0) {
				/* Single dash, not an option.  */
				g->OptIndex= index;
				g->OptLetterIndex = 0;
				g->OptionEnds = true;
				optarg_alt = NULL;
				optind_alt = index;
				optopt_alt = GETOPT_ALT_NOMORE;
				return GETOPT_ALT_NOMORE;
			}
			if (strcmp(p, "--") == 0) {
				/* End of option part. */
				index++;
				g->OptIndex = index;
				g->OptLetterIndex = 0;
				g->OptionEnds = true;
				optarg_alt = NULL;
				optind_alt = index;
				optopt_alt = GETOPT_ALT_NOMORE;
				return GETOPT_ALT_NOMORE;
			}
			letter++;
		}

		opt = *(p + letter);
		if (opt == 0) {
			index++;
			g->OptIndex = index;
			g->OptLetterIndex = 0;
			continue;
		}

		find_opt = strchr(optstring, opt);
		if (find_opt == NULL) {
			/* Unknown option. */
			if (opterr_alt) {
				fprintf(stderr, "%s: Unknown option '%c'.\n", p, opt);
			}
			g->OptIndex = index;
			letter++;
			g->OptLetterIndex = letter;
			optarg_alt = NULL;
			optind_alt = index;
			optopt_alt = opt;
			return GETOPT_ALT_UNKNOWN;
		}

		if (*(find_opt + 1) == ':') {
			/* Option with parameter. */
			parg = p + letter + 1;
			if (*parg != '\0') {
				g->OptIndex = index;
				g->OptLetterIndex = letter + 1 + strlen(parg);
				optarg_alt = parg;
				optind_alt = index;
				optopt_alt = opt;
				return opt;
			}
			index++;
			if (index >= argc) {
				if (opterr_alt) {
					fprintf(stderr, "%s: Use option '%c' with parameter.\n", p, opt);
				}
				index = argc;
				g->OptIndex = index;
				g->OptLetterIndex = 0;
				g->OptionEnds = true;
				optarg_alt = NULL;
				optind_alt = argc;
				optopt_alt = opt;
				return GETOPT_ALT_UNKNOWN;
			}
			optarg_alt = argv[index];
			index++;
			g->OptIndex = index;
			g->OptLetterIndex = 0;
			optind_alt = index;
			optopt_alt = opt;
			return opt;
		}

		/* Switch option. */
		g->OptIndex = index;
		letter++;
		g->OptLetterIndex = letter;
		optarg_alt = NULL;
		optind_alt = index;
		optopt_alt = opt;
		return opt;
	}
	/* Orderd arguments. */
	g->OptIndex = index;
	g->OptLetterIndex = 0;
	g->OptionEnds = true;
	optarg_alt = NULL;
	optind_alt = index;
	optopt_alt = GETOPT_ALT_NOMORE;
	return GETOPT_ALT_NOMORE;
}

/*! alternate getopt() function
 *  @note Following features are not implimented.
 *  * First character in optstring is '+' or '-'.
 *  ** Character '+': Stop processing at non-option argument.
 *  ** Character '-': Treat non-option argument(s) as option
 *                    code 1 parameter.
 */
int getopt_alt(int argc, char * const *argv, const char *optstring)
{	GetOptAlt	*g;
	int		opt;

	g = &GetOptAltState;
	opt = GetOptAltGetOpt(g, argc, argv, optstring);
	return opt;
}
