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

PROJECT_VERSION_FILE="src/project/hgversion.inc"

PROJECT_PRODUCT_NAME="Project"
PROJECT_COPYRIGHT="Copyright (C) Year  Author"
PROJECT_BRANCH="project-dev"
PROJECT_REVISION=$(hg id -n):$(hg id -i)

if [ $PROJECT_UNOFFICIAL = "true" ]
then
    PROJECT_REVISION="Unofficial build - based on $PROJECT_REVISION"
fi

PROJECT_LICENSE="GNU GPL, Version 3"
PROJECT_DATE=$($PROJECT_DATEPATH -R)

echo "#define PROJECT_VER_PRODUCT_NAME     \"$PROJECT_PRODUCT_NAME\"" > $PROJECT_VERSION_FILE
echo "#define PROJECT_VER_COPYRIGHT        \"$PROJECT_COPYRIGHT\"" >> $PROJECT_VERSION_FILE
echo "#define PROJECT_VER_VERSION          PROJECT_VERSION" >> $PROJECT_VERSION_FILE
echo "#define PROJECT_VER_BRANCH           \"$PROJECT_BRANCH\"" >> $PROJECT_VERSION_FILE
echo "#define PROJECT_VER_REVISION         \"$PROJECT_REVISION\"" >> $PROJECT_VERSION_FILE
echo "#define PROJECT_VER_LICENSE          \"$PROJECT_LICENSE\"" >> $PROJECT_VERSION_FILE
echo "#define PROJECT_VER_DATE             \"$PROJECT_DATE\"" >> $PROJECT_VERSION_FILE

echo "Updated $PROJECT_VERSION_FILE"
