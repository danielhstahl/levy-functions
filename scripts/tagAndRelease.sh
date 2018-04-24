#!/bin/bash
if [ -z "$1" ]
  then
    echo "Supply a version, eg 'v1'"
    exit
fi
git add .
git commit -m "release $1"
git tag -a "$1" -m "updating to $1"
git push --follow-tags origin backV1