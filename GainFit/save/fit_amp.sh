#!/bin/bash
rm -r ./calib_histograms/amplitude/

# compile c++ fit routine
g++ fit_amp.C `root-config --libs --cflags` -o fit_amp


do_calib() {
	f=calib_vb$1

	# f=77_40SiPM_0_sw2_Calib_lOn_Int590_HV60_TestbeamConfig_210319
	printf "DOING:  %s %s \n" "$f" 

	./fit_amp $f 

}
export -f do_calib


for file in rootfiles/*; do
	#without extension

	string=$file

	prefix="rootfiles/calib_vb"
	string=${string#$prefix} #Remove prefix
	suffix=".root"
	string=${string%$suffix} #Remove suffix

	#printf "%s\n" ${channel_list[@]} | xargs -n 1 -P 1 bash -c "do_calib $string"
	do_calib $string
done

