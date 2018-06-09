#!/bin/bash
echo "This will replace the latest release.  Continue? (Y/N)"
read contin
if [ $contin==Y ]; then
  tagName=$(curl -s https://api.github.com/repos/phillyfan1138/levy-functions/releases/latest | node -pe "JSON.parse(require('fs').readFileSync('/dev/stdin').toString()).tag_name")

  git add .
  git commit -m "release $tagName"
  releaseId=$(curl -s https://api.github.com/repos/phillyfan1138/levy-functions/releases/tags/$tagName | node -pe "JSON.parse(require('fs').readFileSync('/dev/stdin').toString()).id")
  curl -i -u phillyfan1138 -H "User-Agent: levy-functions" -X DELETE https://api.github.com/repos/phillyfan1138/levy-functions/releases/$releaseId
  git tag -d $tagName
  git push --delete origin $tagName
  git tag -a "$tagName" -m "updating to $tagName"
  git push --follow-tags origin master
fi