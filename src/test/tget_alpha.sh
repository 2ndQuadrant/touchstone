#! /bin/sh
#
# Copyright 2016 PostgreSQL Global Development Group
#

testAlpha1()
{
	export LD_LIBRARY_PATH=".."
	COUNT=`./tget_alpha | sort | uniq | wc -l`
	assertEquals "52 letters" 52 $COUNT 
}

. `which shunit2`
