javac -d . RailwaySignaling.java

for m in {2..3}
do
    rm railway_signaling_${m}/railway_signaling_${m}_*.sgrk

    for n in {2..10}
    do
	java benchmark.RailwaySignaling ${m} ${n}
    done
done
