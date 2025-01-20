#!/bin/bash
writefile=$1
writestr=$2
echo "${writefile} ${writestr}"
# echo "${writestr}" > sample.txt

mkdir -p "$(dirname "$writefile")"
touch $writefile
echo "${writestr}" > $writefile