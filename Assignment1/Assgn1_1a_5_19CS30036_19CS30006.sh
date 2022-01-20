a=$1
for i in $(seq 2 $1)
do 
    while [ $((a%i)) -eq 0 ]
    do
        echo $i 
        a=$((a/i))
    done
done
