#!/bin/bash
if [ -z "$1" ]
  then
    echo "Supply a version, eg 'v1'"
    exit
fi
git add .
git commit -m "release $1"
if git tag -a "$1" -m "updating to $1"; then
  git push --follow-tags origin master
else
  echo "Tag already exists.  Overwrite? (Y/N)"
  read contin
  if [ $contin==Y ]; then
    releaseId=$(curl -s https://api.github.com/repos/phillyfan1138/levy-functions/releases/tags/$1 | node -pe "JSON.parse(require('fs').readFileSync('/dev/stdin').toString()).id")
    echo "this is the id: $releaseId"
    curl -i -u phillyfan1138 -H "User-Agent: levy-functions" -X DELETE https://api.github.com/repos/phillyfan1138/levy-functions/releases/$releaseId
    git tag -d $1
    git push --delete origin $1
    git tag -a "$1" -m "updating to $1"
    git push --follow-tags origin master

  fi
fi