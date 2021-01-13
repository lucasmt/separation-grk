MYSELF=$(realpath $0)
MYDIR=${MYSELF%/*}

for i in {1..10}
do
		python ${MYDIR}/cleaning_robots.py ${i} > ${MYDIR}/cleaning_robots_${i}.sgrk
done
