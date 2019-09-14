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
	./tgenerate_analyze ${HOMEDIR}/../../config/pgsql.conf \
			${HOMEDIR}/q1.sql a1.out
	assertEquals "success" 0 $?

	diff -q ${HOMEDIR}/a1.expected a1.out
	assertEquals "diff" 0 $?
}

. `which shunit2`
