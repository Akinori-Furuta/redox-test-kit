/* SPDX-License-Identifier: BSD-2-Clause */
/* getopt() alternate implimentation */
#if (!defined(GETOPT_ALT_H))
#define GETOPT_ALT_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#if (!defined(GETOPT_ALT_C))
#if (defined(GETOPT_ALT_TAKEOVER))
/* Taking over getopt functions and variables. */
#define optarg		optarg_alt
#define optind		optind_alt
#define opterr		opterr_alt
#define optopt		optopt_alt
#define getopt		getopt_alt
#endif /* (defined(GETOPT_ALT_TAKEOVER)) */
#endif /* (defined(GETOPT_ALT_C)) */

/* No more option */
#define GETOPT_ALT_NOMORE	(-1)  /* compatible with getopt() */
/* Unknown option */
#define GETOPT_ALT_UNKNOWN	('?') /* compatible with getopt() */

/*! Alternate getopt() internal state */
typedef struct {
	int		OptIndex;	/*!< index number of argv[] */
	ssize_t		OptLetterIndex; /*!< string index number of option argument "-abc" */
	bool		OptionEnds;	/*!< No more option. */
} GetOptAlt;

extern GetOptAlt GetOptAltState;

extern char	*optarg_alt;
extern int	optind_alt;
extern int	opterr_alt;
extern int	optopt_alt;

void GetOptAltClear(GetOptAlt *g);
void GetOptAltInit(GetOptAlt *g);
bool GetOptAltIsCleared(GetOptAlt *g);
bool GetOptAltIsNoMore(GetOptAlt *g, int argc, char * const *argv);
int GetOptAltGetOpt(GetOptAlt *g, int argc, char * const *argv, const char *optstring);
int getopt_alt(int argc, char * const *argv, const char *optstring);

#endif /* (!defined(GETOPT_ALT_H)) */
