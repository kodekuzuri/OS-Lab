#!/bin/bash
RANDOM=$$
for i in $(seq 1 150)
do
    echo "$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM" >> $1
done
if [[ $(cut -d ',' -f $2 $1) =~ $3 ]]
then
    echo "YES"
else 
    echo "NO"
fi 

