/*
 * Copyright 2019 PostgreSQL Global Development Group
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#include "config.h"
#include "touchstone.h"

#define MAX_BUFFER_LEN 1024
#define MAX_COLS 255

#define TYPE_SEQUENCE 's'
#define TYPE_TEXT 't'

struct sequence_t
{
	long long arg1;
};

struct text_t
{
	int arg1;
	int arg2;
};

union arguments_t
{
	struct sequence_t sequence;
	struct text_t text;
};

struct column_t
{
	char type;
	union arguments_t arguments;
};

struct table_definition_t
{
	long long rows;
	int columns;
	struct column_t column[MAX_COLS];
};

void sequence(char *, long long);

void usage(char *filename)
{
	printf("usage: %s [options]\n", filename);
	printf("  options:\n");
	printf("    -c <int> - number of data file chunks to generate, default: "
			"1\n");
	printf("    -C <int> - specify which chunk to generate\n");
	printf("    -d <char> - column delimiter, default <tab>\n");
	printf("    -f <filename> - data definition file\n");
	printf("    -o <dir> - location to create data file else use stdout\n");
	printf("    -s <int> - set seed, default: random\n");
}

int generate_data(FILE *stream, struct table_definition_t *table,
		char delimiter, int chunks, int chunk)
{
	char str[MAX_BUFFER_LEN];
	int end = table->columns - 1;

	FILE *which;
	FILE *blackhole = NULL;
	long long chunk_size;
	long long chunk_start = 0;
	long long last_row;

	if (chunks > 1) {
		chunk_size = table->rows / (long long) chunks;
		chunk_start = (chunk - 1) * chunk_size;

		blackhole = fopen("/dev/null", "w");
		if (blackhole == NULL) {
			fprintf(stderr, "ERROR: cannot open /dev/null [%d]\n", errno);
			return 2;
		}
		last_row = chunk * chunk_size;
	}
	else
		last_row = table->rows;

	for (long long row = 0; row < last_row; row++) {
		if (row < chunk_start)
			which = blackhole;
		else
			which = stream;
		for (long long col = 0; col < table->columns; col++) {
			switch (table->column[col].type) {
			case TYPE_SEQUENCE:
				sequence(str, row +
						((struct sequence_t *)
								&table->column[col].arguments)->arg1);
				fprintf(which, "%s", str);
				break;
			case TYPE_TEXT:
				get_alpha(str,
						((struct text_t *)
								&table->column[col].arguments)->arg1,
						((struct text_t *)
								&table->column[col].arguments)->arg2);
				fprintf(which, "%s", str);
				break;
			default:
				fprintf(stderr, "ERROR: unhandled column definition: %c\n",
						table->column[col].type);
				return 1;
			}
			if (col < end)
				fprintf(which, "%c", delimiter);
		}
		fprintf(which, "\n");
		fflush(which);
	}

	if (blackhole != NULL)
		fclose(blackhole);

	return 0;
}

int read_data_definition_file(struct table_definition_t *table, char *filename)
{
	FILE *f;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	int *column;
	int rc;

	fprintf(stderr, "reading %s\n", filename);

	f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr, "ERROR: cannot open data definition file [%d]: %s\n",
				errno, filename);
		return 1;
	}

	/* Read just the first line for the table cardinality. */

	nread = getline(&line, &len, f);
	if (nread == -1) {
		fprintf(stderr, "ERROR: no data on first line of ddf?: %d",
				errno);
		return 2;
	}
	errno = 0;
	table->rows = strtoll(line, NULL, 10);
	if (errno != 0 || table->rows == 0) {
		fprintf(stderr, "ERROR: number of rows invalid [errno %d]: %s\n",
				errno, line);
		free(line);
		fclose(f);
		return 5;
	}

	fprintf(stderr, "%lld row(s)\n", table->rows);

	/* Read the column definition from rest of the file. */

	fprintf(stderr, "column definitions:\n");
	while ((nread = getline(&line, &len, f)) != -1) {
		column = &table->columns;

		if (nread == 0) {
			fprintf(stderr, "ERROR: empty line in data definition file file, "
					"aborting\n");
			free(line);
			fclose(f);
			return 4;
		}

		fprintf(stderr, "[%d] %s", table->columns + 1, line);

		table->column[table->columns].type = line[0];
		switch (line[0]) {
		case TYPE_SEQUENCE:
			rc = sscanf(line + 1, "%d",
					&((struct text_t *)
							&table->column[*column].arguments)->arg1);
			if (rc != 1) {
				fprintf(stderr, "ERROR: invalid argument to sequence: %s\n",
						line + 1);
				free(line);
				fclose(f);
				return 7;
			}
			break;
		case TYPE_TEXT:
			rc = sscanf(line + 1, "%d,%d",
					&((struct text_t *)
							&table->column[*column].arguments)->arg1,
					&((struct text_t *)
							&table->column[*column].arguments)->arg2);
			if (rc != 2) {
				fprintf(stderr, "ERROR: invalid argument to text: %s\n",
						line + 1);
				free(line);
				fclose(f);
				return 7;
			}
			break;
		default:
			fprintf(stderr, "ERROR: unrecognized column definition: %s\n",
					line);
			free(line);
			fclose(f);
			return 6;
		}

		++(*column);
	}

	free(line);
	fclose(f);

	if (table->columns == 0) {
		fprintf(stderr, "ERROR: no columns defined\n");
		return 3;
	}

	if (table->columns > MAX_COLS) {
		fprintf(stderr, "ERROR: more than %d columns defined\n", MAX_COLS);
		return 3;
	}

	fprintf(stderr, "%d column(s)\n", *column);

	return 0;
}

void sequence(char *result, long long value)
{
	int rc;

	rc = snprintf(result, MAX_BUFFER_LEN - 1, "%lld", value);
#ifdef ENABLE_CASSERT
	if (rc < 0 || rc == MAX_BUFFER_LEN - 1)
		fprintf(stderr, "WARNING: sequence issue converting %lld to string\n",
				value);
#endif /* ENABLE_CASSERT */
}

int main(int argc, char *argv[])
{
	int c;
	unsigned long long seed = -1;
	struct table_definition_t table;
	char datafile[FILENAME_MAX] = "";

	char tmp[FILENAME_MAX];
	char *p;

	FILE *stream = stdout;
	int chunk = 0;
	int chunks = 0;
	char delimiter = '\t';
	char data_definition_file[FILENAME_MAX] = "";
	char outdir[FILENAME_MAX] = "";

	memset(&table, 0, sizeof(struct table_definition_t));

	if (argc == 1) {
		usage(argv[0]);
		return 1;
	}

	while (1) {
		int option_index = 1;
		static struct option long_options[] = {
			{0, 0, 0, 0,}
		};

		c = getopt_long(argc, argv, "c:C:d:f:ho:s:",
				long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			break;
		case 'c':
			chunks = atoi(optarg);
			break;
		case 'C':
			chunk = atoi(optarg);
			break;
		case 'd':
			delimiter = optarg[0];
			break;
		case 'f':
			strncpy(data_definition_file, optarg, FILENAME_MAX - 1);
			break;
		case 'h':
			usage(argv[0]);
			return 0;
		case 'o':
			strncpy(outdir, optarg, FILENAME_MAX - 1);
			break;
		case 's':
			seed = atoll(optarg);
			break;
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
			return 2;
		}
	}

	if (data_definition_file[0] == '\0') {
		fprintf(stderr, "ERROR: use -f to specify data definition file\n");
		return 3;
	}

	if (seed == -1) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		seed = getpid();
		seed ^= tv.tv_sec ^ tv.tv_usec;
	}

	if (outdir[0] != '\0') {
		/* Naively remove any extension to the data definition file. */
		strncpy(tmp, data_definition_file, FILENAME_MAX -1);
		p = strstr(tmp, ".");
		*p = '\0';

		/* Make sure the new filename doesn't exceed FILENAME_MAX. */
		c = FILENAME_MAX - (strlen(outdir) + strlen(tmp) + 7);
		if (chunks > 1) {
			--c;
			sprintf(datafile, "%d", chunks);
			c -= strlen(datafile);
		}
		if (c < 0) {
			fprintf(stderr, "ERROR: resulting datafile path and name is too "
					"long: %s/%s.data\n", outdir, tmp);
			return 6;
		}

		strcpy(datafile, outdir);
		strcat(datafile, "/");
		strcat(datafile, tmp);
		if (chunks > 1) {
			sprintf(tmp, "%d", chunk);
			strcat(datafile, ".");
			strcat(datafile, tmp);
		}
		strcat(datafile, ".data");

		stream = fopen(datafile, "w");
		if (stream == NULL) {
			fprintf(stderr, "ERROR: cannot open datafile [%d]: %s\n",
					errno, datafile);
			return 7;
		}

		fprintf(stderr, "datafile: %s\n", datafile);
	}

	if (chunks > 1 && chunk < 1) {
		fprintf(stderr, "ERROR: must specify which chunk to create with -C\n");
		return 8;
	}

	c = read_data_definition_file(&table, data_definition_file);
	if (c != 0)
		return 4;

	init_genrand64(seed);
	c = generate_data(stream, &table, delimiter, chunks, chunk);
	if (c != 0)
		return 5;
	if (outdir[0] != '\0') {
		fclose(stream);
	}

	return 0;
}
