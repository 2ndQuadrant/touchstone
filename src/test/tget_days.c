/*
 * Copyright 2016 PostgreSQL Global Development Group
 */

#include <stdio.h>

#include "touchstone.h"

int main()
{
	int year;

	for (year = 2001; year <= 2004; year++)
		printf("%d: %d days\n", year, get_days(year));

	return 0;
}
