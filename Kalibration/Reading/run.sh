#!/bin/bash
#usage: ./run.sh runNr
#       where runNr is a run number from runlist

here=$(pwd)
if [ ! -d "$here/runs" ]; then
  mkdir $here/runs
fi

runNr=$1 #VOLTAGE
echo $runNr

for f in $here/data/*; do
  echo "Processing $f file..."

done

for folder in $here/data/*; do
  [[ $folder == \#* ]] && continue
  voltageNumber="${folder//[!0-9]/}"
  IFS='/'                    # / is set as delimiter
  read -ra ADDR <<<"$folder" # folder is read into an array as tokens separated by IFS
  IFS=""
  runName=${ADDR[-1]} #last element

  if [ "${voltageNumber}" = "$runNr" ]; then
    echo "RUNNAME: $runName"
    mkdir $here/runs/$runName
    if [ ! -e $here/runs/$runName/$runName.list ]; then
      ls $here/data/$runName | grep \.bin >$here/runs/$runName/$runName.list
    fi
    time $here/read $here/runs/$runName/$runName.list $here/data/$runName/ $here/runs/$runName/out.root $runName
  fi
done

# #valgrind --trace-children=yes --tool=massif time ./../read ./$1/$1.list ./../data/$1/ ./$1/$1.root
# #run for memory check
# #HEAPCHECK=normal LD_PRELOAD=/usr/lib/libtcmalloc.so ./../read ./$1/$1.list ./../data/$1/ ./$1/$1.root
