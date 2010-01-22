#!/bin/sh

cd src/

echo "$(hg tip)\n"

LINES_MAIN=`wc *.sp -l | cut -d ' ' -f1`
LINES_OTHER=`wc project/*.* -l | tail -n1 | sed 's/^ *\(.*\) *$/\1/' | cut -d ' ' -f1`

LINES_TOTAL="$(($LINES_MAIN + $LINES_OTHER))"

echo "Number of lines:"
echo "$LINES_MAIN\tmain sp"
echo "$LINES_OTHER\tother"

echo "\nTotal:"
echo "$LINES_TOTAL"
