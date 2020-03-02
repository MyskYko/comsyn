for i in `seq 6 2 8`
do
    j=3
    jj=`expr $j \* $j`
    jjj=`expr $jj + 2`
    for k in `seq $jj $jjj`
    do
	for s in `seq 2 4`
	do
	    l=`expr $j - 1`
	    mkdir m${i}_${j}_${k}_$s
	    cd m${i}_${j}_${k}_$s
	    python ../../gen_cnn_setting.py $i $i $j $j $k $l $l $s onehot_spx_in systolic
	    gen_cnf setting.txt &
	    cd ..
	done
    done
done
