mkdir -p files_mod 
for f in *.txt
do
    sed "s/ /,/g" $f | awk '{print NR","$0}' > ./files_mod/$f   
done
