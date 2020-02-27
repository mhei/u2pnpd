// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright © 2015 Michael Heimpold <mhei@heimpold.de>
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

#include "options.h"
#include "config.h"

extern char *optarg;
extern int optind;

config_options_t *options = NULL;

config_options_t *options_init(void)
{
	config_options_t *options;

	/* alloc memory and zero it */
	options = calloc(1, sizeof(*options));
	if (!options)
		return NULL;

	return options;
}

void options_free(config_options_t *options)
{
	if (options)
		free(options);
}

/* command line options */
const struct option long_options[] = {
	{ "friendlyName",       required_argument,      0,      'f' },
	{ "manufacturer",       required_argument,      0,      'M' },
	{ "manufacturerURL",    required_argument,      0,      'm' },
	{ "modelDescription",   required_argument,      0,      'D' },
	{ "modelName",          required_argument,      0,      'P' },
	{ "modelNumber",        required_argument,      0,      'N' },
	{ "modelURL",           required_argument,      0,      'p' },
	{ "serialNumber",       required_argument,      0,      'S' },
	{ "uuid",               required_argument,      0,      'u' },
	{ "httpsURL",           no_argument,            0,      's' },
	{ "interface",          required_argument,      0,      'i' },

	{ "version",            no_argument,            0,      'V' },
	{ "help",               no_argument,            0,      'h' },
	{} /* stop condition for iterator */
};

/* descriptions for the command line options */
const char *long_options_descs[] = {
	"a short user-friendly title (default: [manufacturer] [modelName])",
	"manufacturer name",
	"URL to manufacturer site",
	"long user-friendly title",
	"model name",
	"model number",
	"URL to manufacturer's product pages",
	"manufacturer's serial number",
	"UUID of this device",
	"generate a HTTPS URL for the presentation URL (HTTP is used by default)",
	"network interface to use",

	"print version and exit",
	"print this usage and exit",
	NULL /* stop condition for iterator */
};

void usage(char *p, int exitcode)
{
	const char **desc = long_options_descs;
	const struct option *op = long_options;

	fprintf(stderr,
	        "%s (%s) -- tool to announce this device via UPnP on the network\n\n"
	        "Usage: %s [<options>]\n\n"
	        "Options:\n",
	        p, PACKAGE_STRING, p);

	while (op->name && desc) {
		fprintf(stderr, "\t-%c, --%-12s\t%s\n", op->val, op->name, *desc);
		op++; desc++;
	}

	fprintf(stderr, "\n");

	exit(exitcode);
}

/* parse options from the command line */
int options_parse_cli(int argc, char *argv[], config_options_t *options)
{
	int rc = EXIT_FAILURE;

	while (1) {
		int c = getopt_long(argc, argv, "f:M:m:D:P:N:p:S:u:si:Vh", long_options, NULL);

		/* detect the end of the options */
		if (c == -1) break;

		switch (c) {
			case 'f': options->friendlyName = optarg; break;
			case 'M': options->manufacturer = optarg; break;
			case 'm': options->manufacturerURL = optarg; break;
			case 'D': options->modelDescription = optarg; break;
			case 'P': options->modelName = optarg; break;
			case 'N': options->modelNumber = optarg; break;
			case 'p': options->modelURL = optarg; break;
			case 'S': options->serialNumber = optarg; break;
			case 'u': options->uuid = optarg; break;
			case 's': options->use_https = 1; break;
			case 'i': options->interface = optarg; break;

			case 'V':
				printf("%s (%s)\n", argv[0], PACKAGE_STRING);
				exit(EXIT_SUCCESS);
			case '?':
			case 'h':
				rc = EXIT_SUCCESS;
				/* fall-through */
			default:
				usage(argv[0], rc);
		}
	}

	return 0;
}
