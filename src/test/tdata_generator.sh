#! /bin/sh
#
# Copyright 2019 PostgreSQL Global Development Group
#

oneTimeSetUp() {
	export PATH=$PWD/../bin:/usr/bin:/bin
	export LD_LIBRARY_PATH=".."
}

testCheckRowCount() {
	local TABLE="two-rows"
	touchstone-generate-table-data -f ${TABLE}.ddf -o $SHUNIT_TMPDIR
	ROWS=`wc -l $SHUNIT_TMPDIR/${TABLE}.data | cut -d " " -f 1`
	assertEquals "rows" 2 $ROWS
}

testCheckRowFormat() {
	touchstone-generate-table-data -f invalid-row-line.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testChunkNotSpecified() {
	touchstone-generate-table-data -f two-rows.ddf -c 2
	assertEquals "touchstone-generate-table-data" 8 $?
}

testChunkOne() {
	local TABLE="two-rows"
	touchstone-generate-table-data -f ${TABLE}.ddf -c 2 -C 1 -o $SHUNIT_TMPDIR
	ROWS=`wc -l $SHUNIT_TMPDIR/${TABLE}.1.data | cut -d " " -f 1`
	assertEquals "rows" 1 $ROWS
}

testChunkTwo() {
	local TABLE="two-rows"
	touchstone-generate-table-data -f ${TABLE}.ddf -c 2 -C 2 -o $SHUNIT_TMPDIR
	ROWS=`wc -l $SHUNIT_TMPDIR/${TABLE}.2.data | cut -d " " -f 1`
	assertEquals "rows" 1 $ROWS
}

testDataFileCreated() {
	local TABLE="two-rows"
	touchstone-generate-table-data -f ${TABLE}.ddf -o $SHUNIT_TMPDIR
	assertTrue 'data file exists' "[ -f $SHUNIT_TMPDIR/${TABLE}.data ]"
}

testEmptyDataDefinitionFile() {
	touchstone-generate-table-data -f empty.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testInvalidColumnDefinition() {
	touchstone-generate-table-data -f invalid-column-definition.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testMissingDataDefinitionFile() {
	touchstone-generate-table-data -f doesnotexist
	assertEquals "touchstone-generate-table-data" 4 $?
}

testNoColumns() {
	touchstone-generate-table-data -f no-columns.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

testNoCommandLineArguments() {
	touchstone-generate-table-data
	assertEquals "touchstone-generate-table-data" 1 $?
}

testTooManyColumnsDefined() {
	touchstone-generate-table-data -f too-many-columns.ddf
	assertEquals "touchstone-generate-table-data" 4 $?
}

. `which shunit2`
