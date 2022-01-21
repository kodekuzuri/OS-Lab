#!/bin/bash
RANDOM=$$
for i in $(seq 1 150)
do
    echo "$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM,$RANDOM" >> $1
done
cut -d ',' -f $2 $1 

# matching pending 
