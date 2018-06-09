#!/bin/bash
tagID=$(curl -s https://api.github.com/repos/phillyfan1138/levy-functions/releases/latest | node -pe "parseInt(JSON.parse(require('fs').readFileSync('/dev/stdin').toString()).tag_name.replace('v', ''))+1")

tagName="v$tagID"

git add .
git commit -m "release $tagName"
git tag -a "$tagName" -m "updating to $tagName"
git push --follow-tags origin master