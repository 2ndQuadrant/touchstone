% GENRAND64_INT64(3)
% Makoto Matsumoto <m-mat@math.sci.hiroshima-u.ac.jp>, Takuji Nishimura
% 23 February 2005

# NAME

genrand64_int64, init_genrand64

# SYNOPSIS

```c
#include <touchstone.h>

unsigned long long genrand64_int64(void);

long long genrand64_int63(void);

double genrand64_real1(void);

double genrand64_real2(void);

double genrand64_real3(void);

void init_genrand64(unsigned long long seed);

void init_by_array64(unsigned long long init_key[],
             unsigned long long key_length);
```

# DESCRIPTION

This is a 64-bit version of Mersenne Twister pseudorandom number generator.

Before using, initialize the state by using init_genrand64(seed) or
init_by_array64(init_key, key_length).

# RETURN VALUE

The **genrand64_int64()** function returns a value between 0 and 2^64-1 ([0,
2^64-1]).  The **init_genrand64()** functions returns no value.

The **genrand64_int63()** function returns a value [0, 2^63-1].

The **genrand64_real1()** function returns a value [0,1].

The **genrand64_real2()** function returns a value [0,1).

The **genrand64_real3()** function returns a value (0,1).

# SEE ALSO

**collectd**(1), **pidstat**(1), **sar**(1), **sadf**(1)
