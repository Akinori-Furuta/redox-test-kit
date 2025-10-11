#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt-alt.h"

const char *bool_syms[] = {"false", "true"};
const char null_sym[] = "<NULL>";

int main(int argc, char **argv, char **env)
{	bool	opt_a = false;
	bool	opt_b = false;
	bool	opt_c = false;
	char	*opt_s = NULL;
	char	*opt_t = NULL;
	char	*opt_u = NULL;
	int	result = 0;
	int	opt;
	int	i;

	while ((opt = getopt_alt(argc, argv, "abcs:t:u:")) != -1) {
		switch (opt) {
		case 'a':
			opt_a = true;
			break;
		case 'b':
			opt_b = true;
			break;
		case 'c':
			opt_c = true;
			break;
		case 's':
			opt_s = optarg_alt;
			break;
		case 't':
			opt_t = optarg_alt;
			break;
		case 'u':
			opt_u = optarg_alt;
			break;
		case '?':
			fprintf(stdout, "Unknown or required parameter option '%c'\n", optopt_alt);
			break;
		}
	}

	fprintf(stdout, "a=%s, b=%s, c=%s, s=%s, t=%s, u=%s", 
		bool_syms[(ssize_t)opt_a],
		bool_syms[(ssize_t)opt_b],
		bool_syms[(ssize_t)opt_c],
		opt_s ? opt_s : null_sym,
		opt_t ? opt_t : null_sym,
		opt_u ? opt_u : null_sym
	);

	i = optind_alt;
	while (i < argc) {
		fprintf(stdout, ", [%d]=\"%s\"", i, argv[i]);
		i++;
	}
	fprintf(stdout, "\n");
	return result;
}
