MYSELF=$(realpath $0)
MYDIR=${MYSELF%/*}

PREFIX=$1

for file in ${MYDIR}/${PREFIX}/${PREFIX}*.sgrk
do
    python ${MYDIR}/to_strix.py ${file} > ${file}.strix
done
