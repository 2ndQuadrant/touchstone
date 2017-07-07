/*
 * Copyright 2016-2017 PostgreSQL Global Development Group
 */

#ifndef _TOUCHSTONE_H_
#define _TOUCHSTONE_H_

#define ALPHA_LEN 52
#define BUFFER_LEN 64

#define MAXVAR 9
#define VAR_LEN 256

#define int64 long int

struct query_t {
	int id;
	int vars; /* Number of variables we have data for. */
	char comment[BUFFER_LEN]; /* SQL one line comment prefix. */
	char end[BUFFER_LEN]; /* End transaction. */
	char start[BUFFER_LEN]; /* Start transaction */
	/* This is currently hard coded to be able to hold up to 9 variables. */
	char var[MAXVAR][VAR_LEN];
};

int generate_query(char *, char *, struct query_t *);
void get_alpha(char *, int, int);
int get_days(int);
int64 getExponentialRand(int64, int64, double);
int64 getGaussianRand(int64, int64, double);
int64 getPoissonRand(int64);
int64 getrand(int64, int64);
void init_genrand64(unsigned long long);
int load_query_parameters(char *, struct query_t *);

#endif /* _TOUCHSTONE_H_ */
