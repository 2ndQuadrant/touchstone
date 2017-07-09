/*
 * Copyright 2016-2017 PostgreSQL Global Development Group
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "touchstone.h"

const char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void get_alpha(char *str, int min, int max)
{
	int length;
	int i;

	length = min + getrand(min, max);
	str[length - 1] = '\0';
	for (i = 0; i < length - 1; i++)
		str[i] = alpha[(int) getrand(0, ALPHA_LEN - 1)];
}

int get_days(int year)
{
	time_t tloc1, tloc2;
	struct tm tm;

	bzero(&tm, sizeof(struct tm));

	/* Calculate the number of days to generate rows for. */

	tm.tm_year = 1900 - year;
	tm.tm_mday = 1;
	tloc1 = mktime(&tm);

	tm.tm_year += 1;
	tloc2 = mktime(&tm);

	return (int) (difftime(tloc2, tloc1) / 86400.0);
}

int generate_query(char *in, char *out, struct query_t *q)
{
	FILE *fin, *fout;
	int c;
	int i;
	int rc;

	fin = fopen(in, "r");
	if (fin == NULL) {
		fprintf(stderr, "cannot open query input file: %s\n", in);
		return 1;
	}
	fout = fopen(out, "w");
	if (fin == NULL) {
		fprintf(stderr, "cannot open query outpu file: %s\n", out);
		return 2;
	}

	/*
	 * Create a header in the generated query file identifying the query and
	 * the generated varaibled.
	 */
	rc = fprintf(fout, "%s Q %d\n", q->comment, q->id);
	if (rc < 0) {
		fprintf(stderr, "error writing to %s\n", out);
		return 4;
	}
	for (i = 0; i < q->vars; i++) {
		rc = fprintf(fout, "%s %d: %s\n", q->comment, i + 1, q->var[i]);
		if (rc < 0) {
			fprintf(stderr, "error writing to %s\n", out);
			return 4;
		}
	}

	while((c = fgetc(fin)) != EOF) {
		if (c == ':') {
			c = fgetc(fin);
			switch (c) {
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				i = c - 48;
				if (q->vars >= i)
					rc = fprintf(fout, "%s", q->var[i - 1]);
				else {
					fprintf(stderr,
							"Variable %d not defined, only %d exists\n",
							i, q->vars);
					fclose(fout);
					fclose(fin);
					return 3;
				}
				break;
			case 'e':
				rc = fprintf(fout, "%s", q->end);
				break;
			case 'p':
				if (q->flag_analyze)
					rc = fprintf(fout, "%s", q->analyze);
				else if (q->flag_plan)
					rc = fprintf(fout, "%s", q->plan);
				break;
			case 's':
				rc = fprintf(fout, "%s", q->start);
				break;
			default:
				putc(c, fout);
				break;
			}
			if (rc < 0) {
				fprintf(stderr, "error writing to %s\n", out);
				return 4;
			}
		} else {
			putc(c, fout);
		}
	}

	fclose(fout);
	fclose(fin);

	return 0;
}

/*
 * Parameters are in the format of:
 *
 * PARAM: VALUE
 *
 * That parsing is very basic and splits the string on the colon (:) and trims
 * leading spaces on the VALUE.
 */
int load_query_parameters(char * in, struct query_t *q)
{
	FILE *fin;
	char *c, *d;
	char buffer[BUFFER_LEN];
	char param[BUFFER_LEN];
	char value[BUFFER_LEN];

	fin = fopen(in, "r");
	if (fin == NULL) {
		fprintf(stderr, "failed to open: %s\n", in);
		return 1;
	}

	memset(&buffer, 0, BUFFER_LEN);
	while((fgets(buffer, BUFFER_LEN - 1, fin)) != NULL) {
		c = strstr(buffer, ":");
		if (c == NULL) {
			printf("invalid config: %s\n", buffer);
			continue;
		}
		memset(&param, 0, BUFFER_LEN);
		memset(&value, 0, BUFFER_LEN);
		*c = '\0';
		strncpy(param, buffer, BUFFER_LEN - 1);

		/* Strip leading spaces from value. */
		while (*(++c) == ' ' && *c != '\n') ;

		/* Don't copy the linebreak. */
		d = strstr(c, "\n");
		*d = '\0';
		strncpy(value, c, BUFFER_LEN - 1);

		/*
		 * Handle all regcognized parameters and just print out ones not
		 * recognized.
		 */
		if (strncmp(param, "ANALYZE", BUFFER_LEN - 1) == 0)
			strncpy(q->analyze, value, BUFFER_LEN - 1);
		else if (strncmp(param, "COMMENT", BUFFER_LEN - 1) == 0)
			strncpy(q->comment, value, BUFFER_LEN - 1);
		else if (strncmp(param, "END", BUFFER_LEN - 1) == 0)
			strncpy(q->end, value, BUFFER_LEN - 1);
		else if (strncmp(param, "PLAN", BUFFER_LEN - 1) == 0)
			strncpy(q->plan, value, BUFFER_LEN - 1);
		else if (strncmp(param, "START", BUFFER_LEN - 1) == 0)
			strncpy(q->start, value, BUFFER_LEN - 1);
		else
			printf("unknown config: %s\n", buffer);

		memset(&buffer, 0, BUFFER_LEN);
	}
	fclose(fin);

	return 0;
}
