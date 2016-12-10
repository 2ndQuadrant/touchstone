/*
 * Copyright 2016 PostgreSQL Global Development Group
 */

#include <stdio.h>

#include "touchstone.h"

int main()
{
	int i;
	char str[2];

	for (i = 0; i < 1000; i ++) {
		get_alpha(str, 1, 1);
		printf("%s\n", str);
	}

	return 0;
}
