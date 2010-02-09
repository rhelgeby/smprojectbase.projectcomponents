#!/bin/sh

# Note: Copy this script to the source code repository and execute it
#       from that location.

# Program for printing date.
PROJECT_DATEPATH='date'

PROJECT_UNOFFICIAL=false

if [ "$1" ]
then
    if [ "$1" = "--unofficial" ]
    then
        PROJECT_UNOFFICIAL=true
    else
        PROJECT_DATEPATH=$1
    fi
fi

PROJECT_VERSION_FILE="src/project/base/hgversion.inc"

PROJECT_REVISION=$(hg id -n):$(hg id -i)

if [ $PROJECT_UNOFFICIAL = "true" ]
then
    PROJECT_REVISION="Unofficial build - based on $PROJECT_REVISION"
fi

PROJECT_DATE=$($PROJECT_DATEPATH -R)

echo "#define PROJECT_MERCURIAL_REVISION         \"$PROJECT_REVISION\"" > $PROJECT_VERSION_FILE
echo "#define PROJECT_MERCURIAL_DATE             \"$PROJECT_DATE\"" >> $PROJECT_VERSION_FILE

echo "Updated $PROJECT_VERSION_FILE"
