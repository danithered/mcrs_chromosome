#!/bin/bash

# general settings

maxnum=4
par_ID=starttest
no_repeats=10

#parameter settings
par_noEA=4
par_maxtime=50000
par_rangePdeg=
par_maxPdeg=
par_ncol=300
par_nrow=300
par_output_interval=
par_save_interval=1000
par_seed=
par_str_pool=IN/src/mappingA4_80.txt
par_outdir=
par_output_filename=
par_savedir=
par_load=
par_seed_file=
par_init_grid=
par_diffusion_rate=
par_ll=
par_sigma=
par_claimEmpty=
par_substitution=
par_insertion=
par_deletion=
par_g=
par_b1=
par_b2=
par_c=
par_Emin=
par_Nmet=8
par_Nrep=8

#other settings

direct="IN"
outdirect="OUT"
savedirect="save"
log="arrayjobs.txt"


#creating stuff
if [ ! -d  $outdirect ]; then
	mkdir $outdirect
fi

#opening parameter file
if [ ! -d  $direct ]; then
	mkdir IN
fi

#find a jobname
try=1
jobname=$par_ID.$try
if [ -e $outdirect/$log ]
then
	while (( $( grep $jobname -c $outdirect/$log ) > 0 ))
	#while [ -e $jobname.sh ]
	do
		try=$((try+1))
		jobname=$par_ID.$try
	done
fi

#maybe it has been used before...
file=$(printf "param_%s" $jobname)
while [ -e $direct/$file ]
do
	file=$(printf "%s_%s" $file $(date +"%T"))
done 
touch $direct/$file

#creating parameter file
./src/paramfile_gen.R $(printf "%s%s" "--par_maxPdeg" $(printf ",%s" "${par_maxPdeg[@]}")) $(printf "%s%s" "--par_rangePdeg" $(printf ",%s" "${par_rangePdeg[@]}")) $(printf "%s%s" "--par_Nrep" $(printf ",%s" "${par_Nrep[@]}")) $(printf "%s%s" "--par_Nmet" $(printf ",%s" "${par_Nmet[@]}")) $(printf "%s%s" "--par_Emin" $(printf ",%s" "${par_Emin[@]}")) $(printf "%s%s" "--par_c" $(printf ",%s" "${par_c[@]}")) $(printf "%s%s" "--par_b2" $(printf ",%s" "${par_b2[@]}")) $(printf "%s%s" "--par_b1" $(printf ",%s" "${par_b1[@]}")) $(printf "%s%s" "--par_g" $(printf ",%s" "${par_g[@]}")) $(printf "%s%s" "--par_deletion" $(printf ",%s" "${par_deletion[@]}")) $(printf "%s%s" "--par_insertion" $(printf ",%s" "${par_insertion[@]}")) $(printf "%s%s" "--par_substitution" $(printf ",%s" "${par_substitution[@]}")) $(printf "%s%s" "--par_claimEmpty" $(printf ",%s" "${par_claimEmpty[@]}")) $(printf "%s%s" "--par_save_interval" $(printf ",%s" "${par_sigma[@]}")) $(printf "%s%s" "--par_ll" $(printf ",%s" "${par_ll[@]}")) $(printf "%s%s" "--par_diffusion_rate" $(printf ",%s" "${par_diffusion_rate[@]}")) $(printf "%s%s" "--par_init_grid" $(printf ",%s" "${par_init_grid[@]}")) $(printf "%s%s" "--par_seed_file" $(printf ",%s" "${par_seed_file[@]}")) $(printf "%s%s" "--par_load" $(printf ",%s" "${par_load[@]}")) $(printf "%s%s" "--par_savedir" $(printf ",%s" "${par_savedir[@]}")) $(printf "%s%s" "--par_output_filename" $(printf ",%s" "${par_output_filename[@]}")) $(printf "%s%s" "--par_outdir" $(printf ",%s" "${par_outdir[@]}")) $(printf "%s%s" "--par_str_pool" $(printf ",%s" "${par_str_pool[@]}")) $(printf "%s%s" "--par_seed" $(printf ",%s" "${par_seed[@]}")) $(printf "%s%s" "--par_save_interval" $(printf ",%s" "${par_save_interval[@]}")) $(printf "%s%s" "--par_output_interval" $(printf ",%s" "${par_output_interval[@]}")) $(printf "%s%s" "--par_nrow" $(printf ",%s" "${par_nrow[@]}")) $(printf "%s%s" "--par_maxtime" $(printf ",%s" "${par_maxtime[@]}")) $(printf "%s%s" "--par_ncol" $(printf ",%s" "${par_ncol[@]}") ) >> $direct/$file

if (( $(wc -w < $direct/$file ) > 0 ))
then 
	echo >> $direct/$file
fi

# creating repeats
if (( no_repeats > 1 ))
then
	cat $direct/$file > $file.temp
	for r in $(seq 2 $no_repeats)
	do
		cat $file.temp >> $direct/$file
	done
	rm $file.temp
fi

# log file check
maxsize=1024

if [ -e $outdirect/$log ]; then
	actualsize=$(du -k "$outdirect/$log" | cut -f 1)
	if [ $actualsize -ge $maxsize ]; then
		cp $outdirect/$log $outdirect/$log$(date +"%T")
		rm $outdirect/$log
		touch $outdirect/$log
	fi
else
	touch $outdirect/$log
fi

#creating job description
touch $outdirect/output_$jobname

#number of runs
num=$(wc -l < $direct/$file)

#starting jobs
for ((i=1; i <= ${num}; i+=1))
do
	src/creat_inic_matrix.sh 4 300 300 80 > IN/mapping_$jobname'_'${i}.txt
	#echo $(sed "${i}q;d" $direct/$file) --par_seed_plus $i --par_ID $jobname'_'${i} --par_load IN/mapping_$jobname'_'${i}.txt '>>' $outdirect/output_$jobname '2>&1'
	nohup ./simulation $(sed "${i}q;d" $direct/$file) --par_seed_plus $i --par_ID $jobname'_'${i} --par_load IN/mapping_$jobname'_'${i}.txt '>>' $outdirect/output_$jobname '2>&1' &
	pid[ $(( i - 1)) ]=$!
	
	#echo pid no $(( i - 2)) is ${pid[ $(( i - 2)) ]}
	#echo pids are ${pid[@]}
	#echo pids are ${pid[@]} numrun is $numrun maxnum is $maxnum
		
	#count how many of the processes still running - if a lot, than stall
	numrun=$(( maxnum + 1 ))
	while [ "$numrun" -ge "$maxnum" ]
	do
		#echo pid indexes are ${!pid[@]} numrun is $numrun maxnum is $maxnum
		sleep 1
		numrun=0
		for c in ${!pid[@]} 
		do
			#echo c is ${c}
			if kill -0 ${pid[c]}; then
				numrun=$(( numrun + 1 ))
			fi
		done
		#echo numrun is $numrun
	done
done

# writing to log
echo $jobname array job started with $num jobs at $(date +"%D %T"). Paramfile: $file, output file: $outdirect/output_$jobname >> $outdirect/$log




