#!/bin/bash

par_ID=argtest

par_maxtime=
par_ncol=(2 3 4)
par_nrow=
par_output_interval=
par_save_interval=
#par_seed=
par_seed_plus=
par_str_pool=
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
par_Nmet=
par_Nrep=


direct="IN"
file=$(printf "param_%s" $par_ID)
#opening parameter file
if [ ! -d  $direct ]; then
	mkdir IN
fi

# if [ -e $direct/$file ]; then
# 	cp $direct/$file $direct/$file$(date +"%T")
# 	rm $direct/$file
# fi 
while [ -e $direct/$file ]
do
	file=$(printf "%s_%s" $file $(date +"%T"))
done 
touch $direct/$file

#creating parameter file
./src/paramfile_gen.R $(printf "%s%s" "--par_ID" $(printf ",%s" "${par_ID[@]}")) $(printf "%s%s" "--par_ncol" $(printf ",%s" "${par_ncol[@]}") ) >> $direct/$file


#creating job description
outdirect="OUT"
savedirect="save"
log="log"

num=$(wc -l < $direct/$file)

if [ ! -d  $outdirect ]; then
	mkdir $outdirect
fi

#so far so good...



if [ ! -d  $savedirect ]; then
	mkdir $savedirect
fi

maxsize=1024

if [ -e $log ]; then
	actualsize=$(du -k "$log" | cut -f 1)
	if [ $actualsize -ge $maxsize ]; then
		cp $log $log$(date +"%T")
		rm $log
		touch $log
	fi
else
	touch $log
fi

try=1
jobname=$jobnamestart.$try
while [ -e $jobname.sh ]
do
	try=$((try+1))
	jobname=$jobnamestart.$try
done
touch $jobname.sh
echo "#!/bin/bash" >>$jobname.sh
echo "#SBATCH -A eletered" >>$jobname.sh
echo "#SBATCH --job-name="$jobname >>$jobname.sh
echo "#SBATCH --time=12-12:00:00" >>$jobname.sh
echo "#SBATCH --array=1-"$num >>$jobname.sh
echo "#SBATCH --no-requeue" >>$jobname.sh
echo "#SBATCH --mail-type=ALL" >>$jobname.sh
echo "#SBATCH --mail-user=danithered@live.com" >>$jobname.sh
echo >>$jobname.sh 
echo 'srun ./progi $(sed "${SLURM_ARRAY_TASK_ID}q;d"' $indirect/$file')' $jobname'_$SLURM_ARRAY_TASK_ID' >>$jobname.sh

chmod +x $jobname.sh




















# #for i in {0..2}
# for m in ${met_neigh_meret[@]}
# do
#   for k in ${kmax[@]}
#   do
#     for b in ${tradeoffEE[@]}
#     do
#       for d in ${diffuzioGyak[@]}
#       do
#         for e in ${enzakt_num[@]}
#         do
#           for r in ${repl_neigh_meret[@]}
#           do
#             for i in {1..1}}
#             do
# 	      #echo 10 10 0 0 0 $(($i+5)) >> file1
# 	      echo $ncol $nrow ${e} ${m} ${r} $ciklusszam $mintavetel_gyak $cellamintavetel_gyak $alapfeltoltes $emax ${b} $tradeoffEK $kmin ${k} $phalal $claimEmpty $sd $spec_limit ${d} $pMutacio $mentes >> $direct/$file
#             done
#           done
#         done
#       done
#     done
#   done
# done
# #$(($diffuzioGyak+$i))







#a=(1 2)
#b=""

#if[ -z "$a" ]
#then 
# 	echo "empty"
# else
# 	echo "not empty"
# fi
# 
# if [ -z "$a" ]
# then 
# 	for x in ${a[@]}
# 	do 
# 		echo $x
# 	done
# fi
# 
# 
# array=("1 2" "A B")
# 
# for list in "${array[@]}"
# do
#     for item in $list
#     do
#         echo $list : $item
#     done
# done
