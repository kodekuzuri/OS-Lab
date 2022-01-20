mkdir -p files_mod 
for file in ./*.txt
do
    a=${file##*/}
    sed "s/ /,/g" $a | awk '{print NR","$0}' > ./files_mod/$a   
done
