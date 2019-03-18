#!/bin/sh

# cd src/test
# for f in profile-*.ddf; do ./profile-test.sh $f; done

which perf > /dev/null 2>&1
if [ $? -ne 0 ]; then
	echo "ERROR: missing perf, aborting"
	exit 1
fi

for FILE in flamegraph.pl stackcollapse-perf.pl; do
	which $FILE > /dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "ERROR: missing $FILE "
				"(https://github.com/brendangregg/FlameGraph)"
		exit 1
	fi
done

FILE=$(basename -- "$1")
FILE="${FILE%.*}"

perf record -a -g -s -o $FILE.data -- \
		../bin/touchstone-generate-table-data -f $1 > /dev/null

perf report -i $FILE.data -n > $FILE-report.txt
perf annotate -l -P -i $FILE.data > $FILE-annotate.txt
perf script -L -i $FILE.data > $FILE-trace.txt

# https://github.com/brendangregg/FlameGraph
perf script -i $FILE.data | stackcollapse-perf.pl > $FILE.folded
flamegraph.pl $FILE.folded > $FILE.svg
