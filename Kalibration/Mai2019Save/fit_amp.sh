#!/bin/bash
rm ./calib_histograms/amplitude/*
# compile c++ fit routine
g++ fit_amp.C `root-config --libs --cflags` -o fit_amp

#################
### CALIB FIT ###
#################
#Für jeden Kanal
do_calib()
{	
	channel=$0
	
	#f=65_8SiPM_0_Calib_lOn_Int550_HV60_TestbeamConfig_210319 
	# f=77_40SiPM_0_sw2_Calib_lOn_Int590_HV60_TestbeamConfig_210319
	f=calib_vb$1
	#f=tt

	./fit_amp $f $channel

}
export -f do_calib

channel_list=(0 1 2 3 4 5 6 7)
# channel_list=(3)

#Hier -P 1 damit das sortiert ist in der Calib_Datei

for file in rootfiles/*; do
#without extension

string=$file;

 prefix="rootfiles/calib_vb";
 string=${string#$prefix}; #Remove prefix
suffix=".root";
string=${string%$suffix}; #Remove suffix
  printf "DOING:  %s \n" "$string"

   printf "%s\n" ${channel_list[@]} | xargs -n 1 -P 1 bash -c "do_calib $string"
done







#################
### DEBUGGING ###
#################

# Test functions for fast debugging with xargs
test_fn()
{
	echo -n "-> "; for a in "$0"; do echo -n "\"$a\" "; done; echo
	# sleep 1 # show xargs parallel mode 
}
export -f test_fn

test_fn2()
{
	echo $0; #echo $1; echo $2; echo $3;
}
export -f test_fn2

# printf "%s\n" ${channel_list[@]} | xargs -n 1 -P 8 bash -c "test_fn2"