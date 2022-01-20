#!/bin/bash

mkdir -p 1.b.files.out
touch 1.b.files.out/{0..874}.txt
for file in 1.b.files/*
do 
	echo $file
	sort -n -o "1.b.files.out/$(basename $file)" "$file"
done
sort -n 1.b.files.out/*.txt | uniq -c | awk '{print $2,$1}' > 1.b.out.txt