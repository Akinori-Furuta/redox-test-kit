#define _GNU_SOURCE
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

const char HelpMessage[] = "%s: INFO: prand [-s seed_value] bytes_to_output\n";

typedef struct {
	bool		Debug;
	bool		Help;
	char		*Argv0;
	long		Seed;
	ssize_t		Length;
} CCommandLine;

CCommandLine	CommandLine = {
	.Debug =	false,
	.Help =		false,
	.Seed = 0,
	.Length = 0,
};

bool CCommandLineParse(CCommandLine *cmdl, int argc, char **argv)
{	int	result = true;
	int	opt;
	long	lval;
	char	*p;
	char	*p2;

	cmdl->Argv0 = argv[0];
	while ((opt = getopt(argc, argv, "s:vh")) != -1) {
		switch (opt) {
		case 's':
			/* Set Random Seed */
			p = optarg;
			p2 = p;
			lval = strtol(p, &p2, 0);
			if (p2 == p) {
				fprintf(stderr, "%s: ERROR: Specify integer to -s (seed) option.\n",
					cmdl->Argv0
				);
				result = false;
			} else {
				if (lval == 0) {
					fprintf(stderr, "%s: NOTICE: Seed value 0 is alias to 1.\n",
						cmdl->Argv0
					);
				}
				cmdl->Seed = lval;
			}
			break;
		case 'v':
			/* Set debug */
			cmdl->Debug = true;
			break;
		case 'h':
		case '?':
		default:
			/* Set help */
			cmdl->Help = true;
			break;
		}
	}
	if (optind >= argc) {
		fprintf(stderr, "%s: ERROR: Specify bytes to output at 1st argument.\n",
			cmdl->Argv0
		);
		result = false;
		return result;
	}

	p = argv[optind];
	p2 = p;
	lval = strtol(p, &p2, 0);
	if (p2 == p) {
		fprintf(stderr, "%s: ERROR: Specify positive integer number to bytes to output.\n",
			cmdl->Argv0
		);
		result = false;
		return result;
	}
	if (lval < 0) {
		fprintf(stderr, "%s: ERROR: Bytes to output can not be negative value.\n",
			cmdl->Argv0
		);
		result = false;
		return result;
	}
	cmdl->Length = lval;
	return result;
}

#define	RANDOM_STATE_SIZE	(256)
#define	RANDOM_BITS		(31)

bool EmitPesudoRand(CCommandLine *cmdl)
{	unsigned char	*buf0 = NULL;
	unsigned char	*p;
	ssize_t		n;
	ssize_t		wlen;
	bool		result = true;

	char			rstate[RANDOM_STATE_SIZE];
#if !defined(__redox__)
	struct random_data	rdata;
#else /* !defined(__redox__) */
	char			*rstate_prev = NULL;
#endif /* !defined(__redox__) */

	n = cmdl->Length;
	buf0 = malloc(n);
	if (!buf0) {
		fprintf(stderr, "%s: ERROR: Can not allocate buffer. n=%ld(0x%lx)\n",
			cmdl->Argv0, (long)n, (long)n
		);
		return false;
	}

	memset(rstate, 0, sizeof(rstate));
#if !defined(__redox__)
	memset(&rdata, 0, sizeof(rdata));
	if (initstate_r((unsigned int)(cmdl->Seed),
			rstate, sizeof(rstate), &rdata) != 0) {
		fprintf(stderr, "%s: ERROR: initstate_r() returns error, %s.\n",
			cmdl->Argv0,
			strerror(errno)
		);
		result = false;
		goto out_free;
	}
#else /* !defined(__redox__) */
	rstate_prev = initstate((unsigned int)(cmdl->Seed), rstate, sizeof(rstate));
#endif /* !defined(__redox__) */

	p = buf0;
	while (n) {
		int32_t		r32 = 0;

#if !defined(__redox__)
		if (random_r(&rdata, &r32) != 0) {
			fprintf(stderr, "%s: ERROR: random_r() returns error, %s.\n",
				cmdl->Argv0,
				strerror(errno)
			);
			result = false;
			goto out_rstate;
		}
#else /* !defined(__redox__) */
		r32 = (int32_t)random();
#endif /* !defined(__redox__) */
		*p = (unsigned char)(r32 >> (RANDOM_BITS - 8));
		p++;
		n--;
	}

	n = cmdl->Length;
	wlen = fwrite(buf0, sizeof(buf0[0]), n, stdout);
	if (wlen != n) {
		fprintf(stderr, "%s: ERROR: Can not complete fwrite(), %s. wlen=%ld, n=%ld\n",
			cmdl->Argv0,
			strerror(errno),
			(long)(wlen), (long)(n)
		);
		result = false;
		goto out_rstate;
	}
out_rstate:
#if !defined(__redox__)
	/* Do nothing. */
out_free:
#else /* !defined(__redox__) */
	setstate(rstate_prev);
#endif /* !defined(__redox__) */
	free(buf0);
	return result;
}

int main(int argc, char **argv, __maybe_unused char **env)
{	int	result = 0;

	if (!CCommandLineParse(&CommandLine, argc, argv) ||
	    CommandLine.Help) {
		fprintf(stderr, HelpMessage, argv[0]);
		return 1;
	}
	if (CommandLine.Debug) {
		fprintf(stderr, "%s: DEBUG: Debug mode. Seed=%ld, Length=%ld\n",
			argv[0],
			(long)(CommandLine.Seed),
			(long)(CommandLine.Length)
		);
	}
	if (!EmitPesudoRand(&CommandLine)) {
		result = 2;
	}
	return result;
}
