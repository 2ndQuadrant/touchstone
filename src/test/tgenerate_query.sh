#! /bin/sh
#
# Copyright 2017 PostgreSQL Global Development Group
#

oneTimeSetUp() {
	export PATH=$PWD/../bin:/usr/bin:/bin
	export LD_LIBRARY_PATH=".."
	export HOMEDIR=`dirname $0`
}

testGenerateQuery() {
	./tgenerate_query ${HOMEDIR}/q.conf ${HOMEDIR}/q1.sql q1.out
	assertEquals "non-existent config file" 2 $?

	./tgenerate_query ${HOMEDIR}/../../config/pgsql.conf ${HOMEDIR}/badq.sql \
			q1.out
	assertEquals "non-existent query file" 1 $?

	./tgenerate_query ${HOMEDIR}/../../config/pgsql.conf ${HOMEDIR}/q1.sql \
			q1.out
	assertEquals "success" 0 $?

	diff -q ${HOMEDIR}/q1.expected q1.out
	assertEquals "diff" 0 $?

	./tgenerate_query ${HOMEDIR}/../../config/pgsql.conf ${HOMEDIR}/q2.sql \
			q2.out
	assertEquals "undefined variable" 3 $?
}

. `which shunit2`
