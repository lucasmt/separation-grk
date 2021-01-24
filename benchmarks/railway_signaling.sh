rm railway_signaling*.sgrk
javac -d . CreateSpec.java

for n in {2..10}
do
		for k in $(seq 2 ${n})
		do
				java benchmark.CreateSpec ${n} ${k}
		done
done
