#! /bin/sh
#
# Copyright 2017 PostgreSQL Global Development Group
#

testGenerateQuery() {
	export LD_LIBRARY_PATH=".."

	./tgenerate_analyze ../../config/pgsql.conf q1.sql a1.out
	assertEquals "success" 0 $?

	diff -q a1.expected a1.out
	assertEquals "diff" 0 $?
}

. `which shunit2`
