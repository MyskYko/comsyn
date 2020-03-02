for i in `seq 4 2 10`
do
    for j in `seq 2 4`
    do
	k=`expr $j \* $j`
	l=`expr $j - 1`
	echo $i $j
	mkdir m${i}_$j
	cd m${i}_$j
	python ../gen_cnn_setting.py $i $i $j $j $k $l $l 1 onehot_spx_in systolic
	../../gen_cnf setting.txt &
	cd ..
    done
done
