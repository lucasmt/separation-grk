MYSELF=$(realpath $0)
MYDIR=${MYSELF%/*}

for i in {1..10}
do
		python ${MYDIR}/multi_mode.py ${i} > ${MYDIR}/multi_mode_${i}.sgrk
done
