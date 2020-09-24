/*
 * Copyright 2016-2017 PostgreSQL Global Development Group
 */

#ifndef _TOUCHSTONE_H_
#define _TOUCHSTONE_H_

#include <time.h>

#include "pcg_variants.h"
#include "entropy.h"

#define ALPHA_LEN 52
#define BUFFER_LEN 64

#define MAXVAR 9
#define VAR_LEN 256

#define int64 long int

struct query_t {
	int flag_analyze;
	int flag_plan;
	int id;
	int vars; /* Number of variables we have data for. */
	char analyze[BUFFER_LEN]; /* Generate query analysis. */
	char comment[BUFFER_LEN]; /* SQL one line comment prefix. */
	char end[BUFFER_LEN]; /* End transaction. */
	char plan[BUFFER_LEN]; /* Generate query plan. */
	char start[BUFFER_LEN]; /* Start transaction */
	/* This is currently hard coded to be able to hold up to 9 variables. */
	char var[MAXVAR][VAR_LEN];
};

int generate_query(char *, char *, struct query_t *);
double genrand64_real2(pcg64f_random_t *);
void get_alpha(pcg64f_random_t *, char *, int, int);
void get_date(pcg64f_random_t *, struct tm *, time_t, time_t);
int get_days(int);
int64 getExponentialRand(pcg64f_random_t *, int64, int64, double);
int64 getGaussianRand(pcg64f_random_t *, int64, int64, double);
int64 getPoissonRand(pcg64f_random_t *, int64);
int64 getrand(pcg64f_random_t *, int64, int64);
int load_query_parameters(char *, struct query_t *);

#endif /* _TOUCHSTONE_H_ */
