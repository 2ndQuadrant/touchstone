#! /bin/sh
#
# Copyright 2019 PostgreSQL Global Development Group
#

oneTimeSetUp() {
	export PATH=$PWD/../bin:/usr/bin:/bin
	export LD_LIBRARY_PATH=".."
	export HOMEDIR=`dirname $0`
}

testCheckRowCount() {
	local TABLE="two-rows"
	touchstone-generate-table-data -f ${HOMEDIR}/${TABLE}.ddf -o $SHUNIT_TMPDIR
	ROWS=`wc -l $SHUNIT_TMPDIR/${TABLE}.data | cut -d " " -f 1`
	assertEquals "rows" 2 $ROWS
}

testCheckRowFormat() {
	touchstone-generate-table-data -f ${HOMEDIR}/invalid-row-line.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testChunk1() {
	local TABLE="two-rows"
	touchstone-generate-table-data -f ${HOMEDIR}/${TABLE}.ddf -c 2 -C 1 -o $SHUNIT_TMPDIR
	ROWS=`wc -l $SHUNIT_TMPDIR/${TABLE}.1.data | cut -d " " -f 1`
	assertEquals "rows" 1 $ROWS
}

testChunk1() {
	local TABLE="two-rows"
	touchstone-generate-table-data -f ${HOMEDIR}/${TABLE}.ddf -c 2 -C 2 -o $SHUNIT_TMPDIR
	ROWS=`wc -l $SHUNIT_TMPDIR/${TABLE}.2.data | cut -d " " -f 1`
	assertEquals "rows" 1 $ROWS
}

testChunkNotSpecified() {
	touchstone-generate-table-data -f ${HOMEDIR}/two-rows.ddf -c 2
	assertEquals "touchstone-generate-table-data" 8 $?
}

testDataFileCreated() {
	local TABLE="two-rows"
	touchstone-generate-table-data -f ${HOMEDIR}/${TABLE}.ddf -o $SHUNIT_TMPDIR
	assertTrue 'data file exists' "[ -f $SHUNIT_TMPDIR/${TABLE}.data ]"
}

testEmptyDataDefinitionFile() {
	touchstone-generate-table-data -f ${HOMEDIR}/empty.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testInvalidColumnDefinitionDate() {
	touchstone-generate-table-data -f ${HOMEDIR}/invalid-column-definition-date.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testInvalidColumnDefinitionExponential() {
	touchstone-generate-table-data -f ${HOMEDIR}/invalid-column-definition-exponential.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testInvalidColumnDefinitionGaussian() {
	touchstone-generate-table-data -f ${HOMEDIR}/invalid-column-definition-gaussian.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testInvalidColumnDefinitionInteger() {
	touchstone-generate-table-data -f ${HOMEDIR}/invalid-column-definition-integer.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testInvalidColumnDefinitionPoisson() {
	touchstone-generate-table-data -f ${HOMEDIR}/invalid-column-definition-poisson.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testInvalidColumnDefinitionSequence() {
	touchstone-generate-table-data -f ${HOMEDIR}/invalid-column-definition-sequence.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testInvalidColumnDefinitionText() {
	touchstone-generate-table-data -f ${HOMEDIR}/invalid-column-definition-text.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testListFileDoesNotExist() {
	touchstone-generate-table-data -f ${HOMEDIR}/list-does-not-exist.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testMissingDataDefinitionFile() {
	touchstone-generate-table-data -f ${HOMEDIR}/doesnotexist
	assertEquals "touchstone-generate-table-data" 4 $?
}

testNoColumns() {
	touchstone-generate-table-data -f ${HOMEDIR}/no-columns.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testNoCommandLineArguments() {
	touchstone-generate-table-data
	assertEquals "touchstone-generate-table-data" 1 $?
}

testTooManyColumnsDefined() {
	touchstone-generate-table-data -f ${HOMEDIR}/too-many-columns.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

. `which shunit2`
