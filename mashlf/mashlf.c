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
/* 1 */	"%s: INFO: mashlf [-v] [-s seed_value] [-i minimum] [-a maximum] [-d distribution]\n"
/* 2 */	"%s: INFO: -v: Debug\n"
/* 3 */	"%s: INFO: -V: Debug switch, e=message to stdout.\n"
/* 4 */	"%s: INFO: -s seed_value: Integer pseudo random seed value\n"
/* 5 */	"%s: INFO: -i minimum_length: Minimum line length\n"
/* 6 */	"%s: INFO: -a maximum_length: Maximum line length\n"
/* 7 */	"%s: INFO: -d distribution: Distribution function (not implemented)\n"
	;

typedef struct {
	bool		Debug;
	bool		Help;
	char		*Argv0;
	unsigned long	Seed;
	ssize_t		MinimumLength;
	ssize_t		MaximumLength;
	char		*Distribution;
} CCommandLine;

CCommandLine	CommandLine = {
	.Debug =	false,
	.Help =		false,
	.Seed = 0,
	.MinimumLength = 0,
	.MaximumLength = 8191,
	.Distribution = NULL,
};

bool CCommandLineParse(CCommandLine *cmdl, int argc, char **argv)
{	bool	result = true;
	int	opt;
	long		lval;
	unsigned long	ulval;
	char	c;
	char	*p;
	char	*p2;

	cmdl->Argv0 = argv[0];
	while ((opt = getopt(argc, argv, "s:i:a:d:vV:h")) != -1) {
		switch (opt) {
		case 's':
			/* Set Random Seed */
			p = optarg;
			p2 = p;
			ulval = strtoul(p, &p2, 0);
			if (p2 == p) {
				fprintf(fpError, "%s: ERROR: Specify unsigned integer to -s (seed) option.\n",
					cmdl->Argv0
				);
				result = false;
			} else {
				cmdl->Seed = ulval;
			}
			break;
		case 'i':
			/* Set Minimum length */
			p = optarg;
			p2 = p;
			lval = strtol(p, &p2, 0);
			if (p2 == p) {
				fprintf(fpError, "%s: ERROR: Specify positive integer to -i (minumum_length) option.\n",
					cmdl->Argv0
				);
				result = false;
			} else {
				if (lval <= 0) {
					fprintf(fpError, "%s: ERROR: Specify positive value to -i (minumum_length) option.\n",
						cmdl->Argv0
					);
					result = false;
				}
				cmdl->MinimumLength = lval;
			}
			break;
		case 'a':
			/* Set Maximum length */
			p = optarg;
			p2 = p;
			lval = strtol(p, &p2, 0);
			if (p2 == p) {
				fprintf(fpError, "%s: ERROR: Specify positive integer to -a (maxumum_length) option.\n",
					cmdl->Argv0
				);
				result = false;
			} else {
				if (lval <= 0) {
					fprintf(fpError, "%s: ERROR: Specify positive value to -a (maxumum_length) option.\n",
						cmdl->Argv0
					);
					result = false;
				}
				cmdl->MaximumLength = lval;
			}
			break;
		case 'd':
			/* Set Distribution. */
			cmdl->Distribution = optarg;
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
			fprintf(fpError, "%s: INFO: Requested help. opt=%c\n",
				cmdl->Argv0, opt
			);
			cmdl->Help = true;
			break;
		}
	}

	if (cmdl->MinimumLength > cmdl->MaximumLength) {
		ssize_t	tmp;

		tmp = cmdl->MinimumLength;
		cmdl->MinimumLength = cmdl->MaximumLength;
		cmdl->MaximumLength = tmp;
	}

	if (optind < argc) {
		fprintf(fpError, "%s: ERROR: There are no ordered argument(s).\n",
			cmdl->Argv0
		);
		result = false;
		return result;
	}

	return result;
}


#define UTF8SEQ_INIT	(0x00)
#define UTF8SEQ_ASC	(0x01)
#define UTF8SEQ_R2	(0x02)
#define UTF8SEQ_R3	(0x03)
#define UTF8SEQ_R4	(0x04)
#define UTF8SEQ_FIELD	(0x08)

typedef struct {
	uint8_t		SeqRange;
	uint8_t		SeqField;
	uint8_t		ReadChar;
	uint8_t		PushedChar;
	ssize_t		CodeCount;
	ssize_t		CodeRawCount;
	uint8_t		CodeRaw[5];
} UTF8Sequencer;

void UTF8SequencerInit(UTF8Sequencer *us)
{	us->SeqRange = UTF8SEQ_INIT;
	us->SeqField = 0;
	us->ReadChar = 0x00;
	us->PushedChar = 0x00;
	us->CodeCount = 0;
	us->CodeRawCount = 0;
	memset(&(us->CodeRaw[0]), 0, sizeof(us->CodeRaw));
}

uint8_t UTF8SeqDetChar(uint8_t c)
{	uint8_t		t;

	if (c <= 0x7f) {
		/* 00000000 .. 01111111 */
		return UTF8SEQ_ASC;
	}

	t = c & 0xc0;
	if (t == 0xc0) {
		/* 11xxxxxx */
		t = c & 0xe0;
		if (t == 0xc0) {
			/* 110xxxxx */
			return UTF8SEQ_R2;
		} else {
			t = c & 0xf0;
			if (t == 0xe0) {
				/* 1110xxxx */
				return UTF8SEQ_R3;
			}
			/* 1111xxxx */
			return UTF8SEQ_R4;
		}
	} else {
		/* 10xxxxxx */
		return UTF8SEQ_FIELD;
	}
	return UTF8SEQ_ASC;
}

size_t UTF8SequencerStep(UTF8Sequencer *us, uint8_t c)
{	uint8_t		pushed;
	uint8_t		r;
	uint8_t		f;
	uint8_t		det;
	ssize_t		rcount;

	r = us->SeqRange;
	f = us->SeqField;
	/* Adjust Sequence State */
	switch (r) {
	case UTF8SEQ_R2: /* A.Rx.R2 */
		if (f >= 1) {
			r = UTF8SEQ_INIT;
			f = 0;
			us->SeqField = f;
			/* @note CodeRawCount will be fixed at R.ASC */
		}
		break;
	case UTF8SEQ_R3: /* A.Rx.R3 */
		if (f >= 2) {
			r = UTF8SEQ_INIT;
			f = 0;
			us->SeqField = f;
			/* @note CodeRawCount will be fixed at R.ASC */
		}
		break;
	case UTF8SEQ_R4: /* A.Rx.R4 */
		if (f >= 3) {
			r = UTF8SEQ_INIT;
			f = 0;
			us->SeqField = f;
			/* @note CodeRawCount will be fixed at R.ASC */
		}
		break;
	case UTF8SEQ_FIELD: /* A.F */
		if (f == 0) {
			r = UTF8SEQ_INIT;
		}
		break;
	default: /* Will not come here. */
		/* Do nothing */
		break;
	}
	/* Pop pushed char, if is is prsent. */
	pushed = us->PushedChar;
	if (pushed != 0x00) {
		/* There is pushed character. */
		/* @note: pushed char range is [0x80 .. 0xff]. */
		us->CodeRawCount = 1;
		us->CodeRaw[0] = pushed; /* P.To0 */
		us->PushedChar = 0x00;
	}

	us->ReadChar = c;
	det = UTF8SeqDetChar(c);
	switch (r) {
	case UTF8SEQ_INIT:
	case UTF8SEQ_ASC: /* R.ASC */
		switch (det) {
		case UTF8SEQ_ASC:
			us->SeqRange = det;
			us->SeqField = 0;
			us->CodeCount = 1;
			us->CodeRawCount = 1;
			us->CodeRaw[0] = c;
			return 1;
		case UTF8SEQ_R2:
		case UTF8SEQ_R3:
		case UTF8SEQ_R4:
			us->SeqRange = det;
			us->SeqField = 0;
			us->CodeCount = 0;
			us->CodeRawCount = 1;
			us->CodeRaw[0] = c;
			return 0;
		case UTF8SEQ_FIELD:
			us->SeqRange = det;
			us->SeqField = 0;
			us->CodeCount = 1;
			us->CodeRawCount = 1;
			us->CodeRaw[0] = c;
			/* Will be adjusted at A.F */
			return 1;
		default: /* Will not come here. */
			us->SeqRange = UTF8SEQ_ASC;
			us->SeqField = 0;
			us->CodeCount = 0;
			us->CodeRawCount = 1;
			us->CodeRaw[0] = c;
			return 1;
		}
	case UTF8SEQ_R2:
	case UTF8SEQ_R3:
	case UTF8SEQ_R4:
		switch (det) {
		case UTF8SEQ_FIELD:
			f++;
			/* Will be adjusted at A.Rx.R2, A.Rx.R3, A.Rx.R4 */
			us->SeqField = f;
			rcount = us->CodeRawCount;
			us->CodeRaw[rcount] = c;
			rcount++;
			us->CodeRawCount = rcount;
			switch (r) {
			case UTF8SEQ_R2:
				if (rcount >= 2) {
					us->CodeCount = 1;
					return rcount;
				}
				break;
			case UTF8SEQ_R3:
				if (rcount >= 3) {
					us->CodeCount = 1;
					return rcount;
				}
				break;
			case UTF8SEQ_R4:
				if (rcount >= 4) {
					us->CodeCount = 1;
					return rcount;
				}
				break;
			default:
				/* Do nothing. */
				break;
			}
			us->CodeCount = 0;
			return 0;
		case UTF8SEQ_ASC:
			us->SeqRange = det;
			us->SeqField = 0;
			us->CodeCount = 2; /* Broken Rx and ASCII */
			rcount = us->CodeRawCount;
			us->CodeRaw[rcount] = c;
			rcount++;
			/* Will be adjusted at R.ASC */
			us->CodeRawCount = rcount;
			return rcount;
		case UTF8SEQ_R2:
		case UTF8SEQ_R3:
		case UTF8SEQ_R4:
			us->SeqRange = det;
			us->SeqField = 0;
			/* Will be poped at P.To0 */
			us->PushedChar = c;
			us->CodeCount = 1; /* Broken Rx */
			rcount = us->CodeRawCount;
			return rcount;
		default: /* Will not come here. */
			break;
		}
		/* Will not come here. */
		us->SeqRange = UTF8SEQ_ASC;
		us->SeqField = 0;
		us->CodeCount = 2; /* Broken Rx and Broken something. */
		rcount = us->CodeRawCount;
		us->CodeRaw[rcount] = c;
		rcount++;
		/* Will be adjusted at R.ASC */
		us->CodeRawCount = rcount;
		return rcount;
	default: /* Will not come here. */
		break;
	}
	/* Will not come here. */
	us->SeqRange = UTF8SEQ_ASC;
	us->SeqField = 0;
	us->CodeCount = 1; /* Broken something. */
	rcount = us->CodeRawCount;
	us->CodeRaw[rcount] = c;
	rcount++;
	/* Will be adjusted at R.ASC */
	us->CodeRawCount = rcount;
	return rcount;
}

typedef struct {
	ssize_t		CutMin;
	double		CutDelta;
	ssize_t		CutLength;
} MashLf;


void MashLfInit(MashLf *mlf, ssize_t min, ssize_t max)
{	mlf->CutMin = min;
	mlf->CutDelta = (max - min) - 1;
	mlf->CutLength = 0;
}

void MashLfRandLineChars(MashLf *mlf)
{	mlf->CutLength = mlf->CutMin + genrand_real2() * mlf->CutDelta;
}

bool MashLfWriteLf(MashLf *mlf)
{	char	buf;
	ssize_t	wlen;

	buf = '\n';
	wlen = (__force_cast ssize_t)fwrite(&buf, sizeof(buf), 1, stdout);
	if (wlen < 1) {
		fprintf(fpError, "stdout: ERROR: Can not write LF, %s.\n",
			strerror(errno)
		);
		return false;
	}
	return true;
}

bool MashLfWriteThrough(MashLf *mlf, const uint8_t *buf, ssize_t buf_len)
{	ssize_t		wlen;

	if (buf_len <= 0) {
		return true;
	}

	wlen = (__force_cast ssize_t)fwrite(buf,
		sizeof(*buf),
		(__force_cast size_t)buf_len, stdout
	);
	if (wlen < buf_len) {
		fprintf(fpError, "stdout: ERROR: Can not write, %s. buf_len=%ld, wlen=%ld\n",
			strerror(errno),
			(long)buf_len, (long)wlen
		);
		return false;
	}
	return true;
}

bool MashLfGrowLine(MashLf *mlf, ssize_t code_count)
{	bool		result = true;
	ssize_t		cut_len;

	cut_len = mlf->CutLength;
	cut_len -= code_count;

	if (cut_len <= 0) {
		MashLfRandLineChars(mlf);
		result = MashLfWriteLf(mlf);
	}

	return result;
}


bool MashLfMain(CCommandLine *cmdl)
{	bool		result = true;
	UTF8Sequencer	u8seq;
	MashLf		mlf;
	ssize_t		rlen;
	ssize_t		craw_len;

	uint8_t		ch;

	UTF8SequencerInit(&u8seq);
	init_genrand((uint32_t)(cmdl->Seed));
	MashLfInit(&mlf, cmdl->MinimumLength, cmdl->MaximumLength);

	MashLfRandLineChars(&mlf);

	while (!feof(stdin)) {
		ch = 0;
		rlen = (__force_cast ssize_t)fread(&ch, sizeof(ch), 1, stdin);
		if (rlen == 0) {
			/* Consider we see end of file
			 * (no more reads from pipe).
			 */
			break;
		}

		craw_len = UTF8SequencerStep(&u8seq, ch);
		if (craw_len > 0) {
			const uint8_t	*craw;
			ssize_t		code_count;

			craw = &(u8seq.CodeRaw[0]);
			code_count = u8seq.CodeCount;
			if (*(craw + craw_len - 1) == '\n') {
				/* LF terminated,
				 * block propagating LF to output.
				 */
				code_count--;
				craw_len--;
			}

			if (!MashLfWriteThrough(&mlf, craw, craw_len)) {
				result = false;
				break;
			}

			if (!MashLfGrowLine(&mlf, code_count)) {
				result = false;
				break;
			}
		}
	}
	return result;
}

int main(int argc, char ** argv, char **env)
{	int	result = 0;
	bool	parse;

	fpError = stderr;

	parse = CCommandLineParse(&CommandLine, argc, argv);
	if (CommandLine.Debug) {
		fprintf(fpError, "%s: DEBUG: Command line. seed=%lu, minimum=%ld, maximum=%ld\n",
			argv[0],
			(unsigned long)(CommandLine.Seed),
			(long)(CommandLine.MinimumLength),
			(long)(CommandLine.MaximumLength)
		);
	}
	if ((!parse) || (CommandLine.Help)) {
		const char	*argv0;

		argv0 = argv[0];
		fprintf(fpError, HelpMessage,
			argv0, argv0, argv0, argv0, argv0,
			argv0, argv0
		);
		return 1;
	}

	if (!MashLfMain(&CommandLine)) {
		result = 2;
	}
	return result;
}
