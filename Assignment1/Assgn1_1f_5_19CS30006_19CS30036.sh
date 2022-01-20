#!/bin/bash

awk -v var=$2 '{print tolower($var)}' $1 | sort | uniq -c | awk '{print $2 ,$1}' | sort -k 2 -r -n -o 1f_output_$2_column.freq
