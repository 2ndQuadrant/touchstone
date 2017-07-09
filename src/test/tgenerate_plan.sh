#! /bin/sh
#
# Copyright 2017 PostgreSQL Global Development Group
#

testGenerateQuery() {
	export LD_LIBRARY_PATH=".."

	./tgenerate_plan ../../config/pgsql.conf q1.sql p1.out
	assertEquals "success" 0 $?

	diff -q p1.expected p1.out
	assertEquals "diff" 0 $?
}

. `which shunit2`
