MYSELF=$(realpath $0)
MYDIR=${MYSELF%/*}

for file in ${MYDIR}/*.sgrk
do
		python ${MYDIR}/to_strix.py ${file} > ${file}.strix
done
