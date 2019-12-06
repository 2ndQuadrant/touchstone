#! /bin/sh
#
# Copyright 2019 PostgreSQL Global Development Group
#

oneTimeSetUp() {
	export TESTDIR=`dirname $0`
}

testStatsCollection() {
	$TESTDIR/../scripts/ts-sysstat -o $SHUNIT_TMPDIR -i 1
	RC=$?
	assertEquals "stats collection started" 0 $RC
	if [ $RC -ne 0 ]; then
		return
	fi

	$TESTDIR/../scripts/ts-sysstat -o $SHUNIT_TMPDIR -s
	RC=$?
	assertEquals "stats collection stop" 0 $RC
}

. `which shunit2`
