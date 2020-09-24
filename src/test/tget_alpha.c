/*
 * Copyright 2016 PostgreSQL Global Development Group
 */

#include <stdio.h>

#include "touchstone.h"

int main()
{
	int i;
	char str[2];

	unsigned long long seed = -1;
	pcg64f_random_t rng;

	entropy_getbytes((void*) seed, sizeof(seed));
	pcg64f_srandom_r(&rng, seed);

	for (i = 0; i < 1000; i ++) {
		get_alpha(&rng, str, 1, 1);
		printf("%s\n", str);
	}

	return 0;
}
