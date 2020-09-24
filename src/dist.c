/*
 * Originally written by Tatsuo Ishii and enhanced by many contributors.
 *
 * Copyright (c) 2000-2016, PostgreSQL Global Development Group
 * ALL RIGHTS RESERVED;
 */

#include <math.h>

#include "touchstone.h"

/* random number generator: uniform distribution from min to max inclusive */
int64
getrand(pcg64f_random_t *rng, int64 min, int64 max)
{
	/*
	 * Odd coding is so that min and max have approximately the same chance of
	 * being selected as do numbers between them.
	 */
	return min + (int64) ((max - min + 1) * genrand64_real2(rng));
}

/*
 * random number generator: exponential distribution from min to max inclusive.
 * the parameter is so that the density of probability for the last cut-off max
 * value is exp(-parameter).
 */
int64
getExponentialRand(pcg64f_random_t *rng, int64 min, int64 max, double parameter)
{
	double		cut,
				uniform,
				rand;

	cut = exp(-parameter);
	/* erand in [0, 1), uniform in (0, 1] */
	uniform = 1.0 - genrand64_real2(rng);

	/*
	 * inner expression in (cut, 1] (if parameter > 0), rand in [0, 1)
	 */
	rand = -log(cut + (1.0 - cut) * uniform) / parameter;
	/* return int64 random number within between min and max */
	return min + (int64) ((max - min + 1) * rand);
}

/* random number generator: gaussian distribution from min to max inclusive */
int64
getGaussianRand(pcg64f_random_t *rng, int64 min, int64 max, double parameter)
{
	double		stdev;
	double		rand;

	/*
	 * Get user specified random number from this loop, with -parameter <
	 * stdev <= parameter
	 *
	 * This loop is executed until the number is in the expected range.
	 *
	 * As the minimum parameter is 2.0, the probability of looping is low:
	 * sqrt(-2 ln(r)) <= 2 => r >= e^{-2} ~ 0.135, then when taking the
	 * average sinus multiplier as 2/pi, we have a 8.6% looping probability in
	 * the worst case. For a parameter value of 5.0, the looping probability
	 * is about e^{-5} * 2 / pi ~ 0.43%.
	 */
	do
	{
		/*
		 * genrand64_real2 generates [0,1), but for the basic version of the
		 * Box-Muller transform the two uniformly distributed random numbers
		 * are expected in (0, 1] (see
		 * http://en.wikipedia.org/wiki/Box_muller)
		 */
		double		rand1 = 1.0 - genrand64_real2(rng);
		double		rand2 = 1.0 - genrand64_real2(rng);

		/* Box-Muller basic form transform */
		double		var_sqrt = sqrt(-2.0 * log(rand1));

		stdev = var_sqrt * sin(2.0 * M_PI * rand2);

		/*
		 * we may try with cos, but there may be a bias induced if the
		 * previous value fails the test. To be on the safe side, let us try
		 * over.
		 */
	}
	while (stdev < -parameter || stdev >= parameter);

	/* stdev is in [-parameter, parameter), normalization to [0,1) */
	rand = (stdev + parameter) / (parameter * 2.0);

	/* return int64 random number within between min and max */
	return min + (int64) ((max - min + 1) * rand);
}

/*
 * random number generator: generate a value, such that the series of values
 * will approximate a Poisson distribution centered on the given value.
 */
int64
getPoissonRand(pcg64f_random_t *rng, int64 center)
{
	/*
	 * Use inverse transform sampling to generate a value > 0, such that the
	 * expected (i.e. average) value is the given argument.
	 */
	double		uniform;

	/* erand in [0, 1), uniform in (0, 1] */
	uniform = 1.0 - genrand64_real2(rng);

	return (int64) (-log(uniform) * ((double) center) + 0.5);
}
