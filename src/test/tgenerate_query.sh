#! /bin/sh
#
# Copyright 2017 PostgreSQL Global Development Group
#

testGenerateQuery() {
	export LD_LIBRARY_PATH=".."

	./tgenerate_query q.conf q1.sql q1.out
	assertEquals "non-existent config file" 2 $?

	./tgenerate_query ../../config/pgsql.conf badq.sql q1.out
	assertEquals "non-existent query file" 1 $?

	./tgenerate_query ../../config/pgsql.conf q1.sql q1.out
	assertEquals "success" 0 $?

	diff -q q1.expected q1.out
	assertEquals "diff" 0 $?

	./tgenerate_query ../../config/pgsql.conf q2.sql q2.out
	assertEquals "undefined variable" 3 $?
}

. `which shunit2`
