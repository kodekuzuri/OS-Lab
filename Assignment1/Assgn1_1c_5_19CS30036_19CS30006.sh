#!/bin/bash
my_array=($(find . -type f | sed -rn 's|.*/[^/]+\.([^/.]+)$|\1|p' | sort -u))
for i in "${my_array[@]}"
do 
	echo "$i"   # Used for debugging
	mkdir -p $i
	find . -name "*.$i" -type f -not -path "./$i/*" -exec mv -n -t "$i" {} \;
done
echo "No extension files" # Used for debugging
mkdir -p "No_Extension"
find . -type f ! -name "*.*" -not -path "./No_Extension/*" -exec mv -n -t "No_Extension" {} + 
