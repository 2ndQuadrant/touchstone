/*
 * Copyright 2017 PostgreSQL Global Development Group
 */

#include <stdio.h>
#include <string.h>

#include "touchstone.h"

int main(int argc, char *argv[])
{
	int rc;
	struct query_t q;

	if (argc != 4) {
		fprintf(stderr, "usage: %s <config> <input> <output>\n", argv[0]);
		return 1;
	}
	rc = load_query_parameters(argv[1], &q);
	if (rc) {
		fprintf(stderr, "load_query_parameters: %d\n", rc);
		return 2;
	}

	q.id = 1;
	q.vars = 1;
	q.flag_analyze = 0;
	q.flag_plan = 1;
	strcpy(q.var[0], "ONE");

	return generate_query(argv[2], argv[3], &q);
}
