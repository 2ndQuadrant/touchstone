#! /bin/sh
#
# Copyright 2016 PostgreSQL Global Development Group
#

testDays() {
	export LD_LIBRARY_PATH=".."
	./tget_days
	assertEquals "success" 0 $?
}

. `which shunit2`
