#!/bin/bash

half=0

for i in 1 2 4 8
do
	for j in 16 32 64 128 256 512
	do
		let half=j/2
		echo "test [$i $j $half]..."
		echo "0 $i $j $half" > config
		./core_simulator config hw2_trace_mcf.out > ./result_mcf/"$i"_"$j"_"$half".out

		echo "test [$i $j $j]..."
		echo "0 $i $j $j" > config
		./core_simulator config hw2_trace_mcf.out > ./result_mcf/"$i"_"$j"_"$j".out
	done
done
