#!/bin/bash 

no_strs=$1

#first line of mapping file (number of activities)
echo $no_strs

#going thru directories
for(( d=1 ; d <= $no_strs ; d++ ))
do	
	#find str files in directory
	strfiles=($(find IN/str/${d}/str*))
	
	#looping thru those files
	for s in ${strfiles[@]}
	do
		#print rule
		printf "1 "
		sed -n 1p $s
		#print subrules
		sed '1d; :a;N;$!ba;s/\n/ /g' $s
		
		#print activities for subrules
		for (( a = 1 ; a <= $no_strs ; a++ ))
		do
			if [ $a = $d ]
			then
				printf "%d " 1
			else
				printf "%d " 0
			fi
		done
		echo
	done
done
