#!/bin/bash
if [ -z "$1" ]
  then
    echo "Supply a version, eg 'v1'"
    exit
fi
git add .
git commit -m "release $1"
if git tag -a "$1" -m "updating to $1";
then
  git push --follow-tags origin master
else
  echo "Tag already exists.  Overwrite? (Y/N)"
  read contin
  if $contin==Y; then
    git tag -d $1
    git push --delete origin $1
    git push --follow-tags origin master
  fi
fi