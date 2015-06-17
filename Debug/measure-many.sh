#!/bin/bash
# Usage: ./measure-many

for seed in {1..100} 
do
	for threads in {1..10} 
	do				
		echo "===================================" >> ../measures.txt
		echo "SEED: ${seed}   THREADS: ${threads}" >> ../measures.txt
		echo "===================================" >> ../measures.txt
		# Run N times each and calculate mean real-time.	
		(time ./Syspro-HW3 100000000 ${threads} 5 9006 ../logfiles/logfile${seed}-${threads}.txt -s ${seed} -noserve -d 0) &> temp.txt
		t=$(tail -3 temp.txt | head -1 | cut -f2)
		echo "${t}" >> ../measures.txt
		# done
		rm temp.txt
	done
done
