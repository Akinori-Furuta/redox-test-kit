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
#include "../mt19937ar/mt19937ar.h"
#define GETOPT_ALT_TAKEOVER
#include "../getopt/getopt-alt.h"

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

/* Error output file pointer.
 */
FILE	*fpError = NULL;

const char HelpMessage[] =
	"%s: INFO: prand [-v] [-s seed_value] bytes_to_output\n"
	"%s: INFO: -v: Debug\n"
	"%s: INFO: -s seed_value: Integer pseudo random seed value\n"
	"%s: INFO: -V debug_switch: e: Output error messages to stdout.\n"
	;

typedef struct {
	bool		Debug;
	bool		Help;
	char		*Argv0;
	unsigned long	Seed;
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
	long		lval;
	unsigned long	ulval;
	char	c;
	char	*p;
	char	*p2;

	cmdl->Argv0 = argv[0];
	while ((opt = getopt(argc, argv, "s:vV:h")) != -1) {
		switch (opt) {
		case 's':
			/* Set Random Seed */
			p = optarg;
			p2 = p;
			ulval = strtoul(p, &p2, 0);
			if (p2 == p) {
				fprintf(fpError, "%s: ERROR: Specify integer to -s (seed) option.\n",
					cmdl->Argv0
				);
				result = false;
			} else {
				cmdl->Seed = ulval;
			}
			break;
		case 'v':
			/* Set debug */
			cmdl->Debug = true;
			break;
		case 'V':
			/* Debug switch */
			p = optarg;
			while ((c = *p) != 0) {
				switch (c) {
				case 'e':
					fpError = stdout;
					break;
				default:
					break;
				}
				p++;
			}
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
		fprintf(fpError, "%s: ERROR: Specify bytes to output at 1st argument.\n",
			cmdl->Argv0
		);
		result = false;
		return result;
	}

	p = argv[optind];
	p2 = p;
	lval = strtol(p, &p2, 0);
	if (p2 == p) {
		fprintf(fpError, "%s: ERROR: Specify positive integer number to bytes to output.\n",
			cmdl->Argv0
		);
		result = false;
		return result;
	}
	if (lval < 0) {
		fprintf(fpError, "%s: ERROR: Bytes to output can not be negative value.\n",
			cmdl->Argv0
		);
		result = false;
		return result;
	}
	cmdl->Length = lval;
	return result;
}

bool EmitPesudoRand(CCommandLine *cmdl)
{	unsigned char	*buf0 = NULL;
	unsigned char	*p;
	ssize_t		n;
	ssize_t		wlen;
	bool		result = true;

	n = cmdl->Length;
	buf0 = malloc(n);
	if (!buf0) {
		fprintf(fpError, "%s: ERROR: Can not allocate buffer. n=%ld(0x%lx)\n",
			cmdl->Argv0, (long)n, (long)n
		);
		return false;
	}

	init_genrand((uint32_t)(cmdl->Seed));

	p = buf0;
	while (n) {
		uint32_t	r32 = 0;

		r32 = genrand_uint32();
		*p = (unsigned char)r32;
		p++;
		n--;
	}

	n = cmdl->Length;
	wlen = fwrite(buf0, sizeof(buf0[0]), n, stdout);
	if (wlen != n) {
		fprintf(fpError, "%s: ERROR: Can not complete fwrite(), %s. wlen=%ld, n=%ld\n",
			cmdl->Argv0,
			strerror(errno),
			(long)(wlen), (long)(n)
		);
		result = false;
		goto out;
	}
out:
	free(buf0);
	return result;
}

int main(int argc, char **argv, __maybe_unused char **env)
{	int	result = 0;
	char	*a0;

	fpError = stderr;

	a0 = argv[0];
	if (!CCommandLineParse(&CommandLine, argc, argv) ||
	    CommandLine.Help) {
		fprintf(fpError, HelpMessage,
			a0,
			a0,
			a0,
			a0
		);
		return 1;
	}
	if (CommandLine.Debug) {
		fprintf(fpError, "%s: DEBUG: Debug mode. Seed=%ld, Length=%ld\n",
			a0,
			(long)(CommandLine.Seed),
			(long)(CommandLine.Length)
		);
	}
	if (!EmitPesudoRand(&CommandLine)) {
		result = 2;
	}
	return result;
}
