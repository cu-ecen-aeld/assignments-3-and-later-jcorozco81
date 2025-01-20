#!/bin/bash
filesdir=$1
searchstr=$2



if [ $# -eq 2 ]
then
if [ -d "$filesdir" ]
then
X=$(find $filesdir -type f | wc -l)
Y=$(grep -R $searchstr $filesdir | wc -l)

echo "The number of files are $X and the number of matching lines are $Y"


else

echo "${filesdir} does not represent a directory on the filesystem" 
exit 1
fi


else
echo "Parameters were not specified"
exit 1
fi

exit 0

