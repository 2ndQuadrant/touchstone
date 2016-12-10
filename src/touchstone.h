/*
 * Copyright 2016 PostgreSQL Global Development Group
 */

#ifndef _TOUCHSTONE_H_
#define _TOUCHSTONE_H_

#define ALPHA_LEN 52

#define int64 long int

void get_alpha(char *, int, int);
int get_days(int);
int64 getExponentialRand(int64, int64, double);
int64 getGaussianRand(int64, int64, double);
int64 getPoissonRand(int64);
int64 getrand(int64, int64);
void init_genrand64(unsigned long long);

#endif /* _TOUCHSTONE_H_ */
