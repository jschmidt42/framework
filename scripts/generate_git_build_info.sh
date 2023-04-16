#!/bin/bash
cd "$(dirname "$0")"

## Automatically generate a file with git branch and revision info
FILENAME='../artifacts/version.git.h'

exec 1>&2
branch=`git rev-parse --abbrev-ref HEAD`
shorthash=`git log --pretty=format:'%h' -n 1`
revcount=`git log --oneline | wc -l`
latesttag=`git describe --tags --abbrev=0 --always`

echo "// Automatically generated by git hook. Do not edit manually." > $FILENAME
echo "" >> $FILENAME
echo "#pragma once" > $FILENAME
echo "" >> $FILENAME
echo "#define GIT_BRANCH \"$branch\"" >> $FILENAME
echo "#define GIT_COMMIT \"$latesttag\"" >> $FILENAME
echo "#define GIT_REVCOUNT $revcount" >> $FILENAME
echo "#define GIT_SHORT_HASH \"$shorthash\"" >> $FILENAME
echo "" >> $FILENAME
