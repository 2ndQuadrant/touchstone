/*
 * Copyright 2016 PostgreSQL Global Development Group
 */

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
