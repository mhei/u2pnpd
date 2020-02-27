// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright © 2015 Michael Heimpold <mhei@heimpold.de>
 *
 */

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define FN_DEVICE_INFO "/etc/device_info"
#define FN_CPUINFO "/proc/cpuinfo"
#define FN_SYSINFO_PREFIX "/tmp/sysinfo"

#define FWPRINTENV "fw_printenv -s"

static inline int line_starts_with(char *line, const char *prefix)
{
	return strncasecmp(line, prefix, strlen(prefix)) == 0;
}

int deviceinfo_from_cpuinfo(char **dest, const char *key)
{
	FILE *fp;
	char *start, *nl, *line = NULL;
	size_t len = 0;
	ssize_t c;
	int rv = -2;

	fp = fopen(FN_CPUINFO, "rb");
	if (fp == NULL)
		return -1;

	while ((c = getline(&line, &len, fp)) != -1) {
		/* skip lines not starting with key */
		if (!line_starts_with(line, key))
			continue;

		/* search ':' */
		start = index(line, ':');
		if (!start)
			continue;
		start++;

		/* trim leading whitespacec */
		while (*start && (*start == ' ' || *start == '\t'))
			start++;

		/* trim away newline character(s) */
		nl = start;
		while (*nl && *nl != '\n' && *nl != '\r')
			nl++;
		*nl = '\0';

		*dest = strdup(start);

		/* string length */
		rv = nl - start;

		/* found, skip remaining file */
		break;
	}

	fclose(fp);
	if (line)
		free(line);

	return rv;
}

static int line_get_value(char **dest, char *line)
{
	char *s, *e;
	char quote = 0;
	int c, q;

	s = index(line, '=');
	if (!s)
		return -1;

	/* point to position after '=' */
	s++;

	/* trim leading white space */
	while (*s && (*s == ' ' || *s == '\t'))
		s++;

	/* remember a possible quote char */
	if (*s == '\'' || *s == '"')
		quote = *s++;

	/* search end of value, quoted quotes may be prefixe with a backslash (...='...\'...') */
	e = s; q = 0;
	while (*e && *e != '\r' && *e != '\n' && (*e != quote || (*e == quote && q == '\\')))
		q = *e++;

	c = e - s;
	if (c == 0) {
		*dest = strdup("");
		return 0;
	}

	/* unescaped quoted chars */
	*dest = e = malloc(c + 1);
	for (q = 0; q < c; q++)
		if (q < (c - 1) && s[q] == '\\' && s[q + 1] == quote)
			*e++ = s[++q];
		else
		    *e++ = s[q];
	*e = '\0';

	/* return string length */
	return e - *dest;
}

int deviceinfo_from_deviceinfo(char **dest, const char *key)
{
	FILE *f;
	char *line = NULL;
	size_t len = 0;
	ssize_t c;
	int rv = -2;

	f = fopen(FN_DEVICE_INFO, "rb");
	if (f == NULL)
		return -1;

	while ((c = getline(&line, &len, f)) != -1) {
		if (!line_starts_with(line, key))
			continue;

		c = line_get_value(dest, line);
		if (c < 0)
			continue;

		/* found so skip remaining file */
		rv = c;
		break;
	}

	fclose(f);
	if (line)
		free(line);

	return rv;
}

int deviceinfo_from_sysinfo(char **dest, const char *key)
{
	FILE *f;
	char *s, *nl, *line = NULL;
	size_t len = 0;
	ssize_t c;
	int rv = -2;

	if (asprintf(&s, "%s/%s", FN_SYSINFO_PREFIX, key) == -1)
		return rv;

	f = fopen(s, "rb");
	free(s);
	if (f == NULL)
		return -1;

	if ((c = getline(&line, &len, f)) == -1) {
		fclose(f);
		return -1;
	}

	s = line;

	/* trim leading whitespacec */
	while (*s && (*s == ' ' || *s == '\t'))
		s++;

	/* trim away newline character(s) */
	nl = s;
	while (*nl && *nl != '\n' && *nl != '\r')
		nl++;
	*nl = '\0';

	*dest = strdup(s);

	/* string length */
	rv = nl - s;

	fclose(f);
	if (line)
		free(line);

	return rv;
}
