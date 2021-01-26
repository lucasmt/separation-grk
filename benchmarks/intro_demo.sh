MYSELF=$(realpath $0)
MYDIR=${MYSELF%/*}

for i in {1..10}
do
		python ${MYDIR}/intro_demo.py ${i} > ${MYDIR}/intro_demo_${i}.sgrk
done
