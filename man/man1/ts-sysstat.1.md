% ts-sysstat(1)
% Mark Wong <mark@2ndQuadrant.com>
% 20 January 2020

# NAME

ts-sysstat - Stats collection management script and post-processor

# SYNOPSIS

ts-sysstat _\[options\]_

# DESCRIPTION

**ts-sysstat** is a wrapper script that starts and stops collecting data using
**sar**, **pidstat**, and **collectd** if they are available on the system and
in the user's path.  This script will save the output of these tools in a
specified directory.

When stopping data collection, the script will also attempt to use **sadf**
with **sar** to generate human readable output files and svg files.  The data
captured from **pidstat** is also massaged to make it easier to be consumed by
programs that can handle CSV-like data.

# OPTIONS

**-i** _\<interval\>_
:   Interval (seconds) between samples, default 60

**-o** _\<path\>_
:   Path to save data

**-s**
:   Stop stat collection processes saving to -o <path>

# SEE ALSO

**collectd**(1), **pidstat**(1), **sar**(1), **sadf**(1)
