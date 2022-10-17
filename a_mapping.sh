#!/bin/bash 

# as I see it creates a mapping file (that mcrs accepts) from structure descriptions which strgen generates
# it`s argoments are the input files
# it outputs to std output, so you may would like to redirect it to a file

echo $# >> temp.vmi

start=1
for input in "$@" 
do
	printf "1 " >> temp.vmi 
	sed -n 1p $input >> temp.vmi
	sed '1d; :a;N;$!ba;s/\n/ /g' $input >> temp.vmi
	for (( a = 1 ; a <= $# ; a++ ))
	do
		if [ $a = $start ]
		then
			printf "%d " 1 >> temp.vmi
		else
			printf "%d " 0 >> temp.vmi
		fi
	done
	echo >> temp.vmi
	start=$((start + 1))
done

cat temp.vmi
rm temp.vmi
