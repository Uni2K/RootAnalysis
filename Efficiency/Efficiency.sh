#!/bin/bash

#################
### COMPILE   ###
#################
cd "${0%/*}"

g++ Efficiency.C -lstdc++fs `root-config --libs --cflags` -o efficiency -lSpectrum 

##################
### INITIALIZE ###
#################

 for file in "../rootfiles/"/*; do
	#without extension

	if [ ! -z "$1" ]; then

		if [[ $file == *"$1"* ]]; then
			echo $line
		else
			continue
		fi

	fi

	rootFilePath=$file

	prefix="../rootfiles//"
	runName=${file#$prefix} #Remove prefix
	suffix=".root"
	runName=${runName%$suffix} #Remove suffix

	runNr=$(echo "$runName" | sed -r 's/^([^.]+).*$/\1/; s/^[^0-9]*([0-9]+).*$/\1/')
	
	./efficiency $runName $runNr $rootFilePath 
done
 


