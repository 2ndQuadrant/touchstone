#! /bin/sh
#
# Copyright 2019 PostgreSQL Global Development Group
#

oneTimeSetUp() {
	export TESTDIR=`dirname $0`

	$TESTDIR/../scripts/ts-sysstat -o $SHUNIT_TMPDIR -i 1
	echo $SHUNIT_TMPDIR
	RC=$?
	assertEquals "stats collection started" 0 $RC
	if [ $RC -ne 0 ]; then
		return 1
	fi

	sleep 8

	$TESTDIR/../scripts/ts-sysstat -o $SHUNIT_TMPDIR -s
	RC=$?
	assertEquals "stats collection stop" 0 $RC
}

testGnuplotExists() {
	which gnuplot > /dev/null 2>&1
	RC=$?
	assertEquals "gnuplot found" 0 $RC
}

testPlottingPidstat() {
	$TESTDIR/../scripts/ts-plot-pidstat -i $SHUNIT_TMPDIR
	RC=$?
	assertEquals "pidstat plotting" 0 $RC
}

testPlottingSar() {
	$TESTDIR/../scripts/ts-plot-sar -i $SHUNIT_TMPDIR/sar
	RC=$?
	assertEquals "pidstat sar" 0 $RC
}

. `which shunit2`
