#!/bin/bash
if [ -z "$1" ]
  then
    echo "Supply a version, eg 'v1'"
    exit
fi
git add .
git commit -m "release v$1"
git tag -a "v$1" -m "updating to v$1"
git push --follow-tags origin master