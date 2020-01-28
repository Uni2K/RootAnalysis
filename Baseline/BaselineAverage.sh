#!/bin/bash


cd "${0%/*}"
cwd=`pwd`


#################
### COMPILE   ###
#################

g++ BaselineAverage.C `root-config --libs --cflags` -o BL_average_new

##################
### INITIALIZE ###
##################



hist_dir="BL_histograms"

if [ ! -d "$cwd/"$hist_dir"" ]; then
		mkdir $cwd/"$hist_dir"
fi



#################
### CALIB FIT ###
#################


for file in "../rootfiles/"/*; do
	#without extension

	rootFilePath=$file
	
	prefix="../rootfiles//"
	runName=${file#$prefix} #Remove prefix
	suffix=".root"
	runName=${runName%$suffix} #Remove suffix
	
	runNr=`echo "$runName" | sed -r 's/^([^.]+).*$/\1/; s/^[^0-9]*([0-9]+).*$/\1/'`
	xLimit=2

	 

	string="$runName $runNr "6075PE" "40-SiPM" "sw5" $xLimit $rootFilePath"
	#echo $string
	#printf "%s\n" ${channel_list[@]} | xargs -n 1 -P 1 bash -c "do_calib $string"
	#"9_calib_vb58_tune8530_pcbb 9 6075PE 40-SiPM sw5 3 83 6050PE 8-ArrayTB17 none 2."

	./BL_average_new $runName $runNr "40-SiPM" "sw5" $xLimit $rootFilePath
done





data_list=(

	## DARK COUNT
	# final analysis datasets, Sep. 2019
	# scheme:
	# filename run_nr sipm_id wom_id sw_id x_limit

	# #// WOM-C/-D,40Array0 sw1

	"9_calib_vb58_tune8530_pcbb 9 6075PE 40-SiPM sw5 3 83 6050PE 8-ArrayTB17 none 2."

	
	## CALIB RUNS
	# final analysis datasets, Sep. 2019
	# scheme:
	# filename run_nr sipm_id wom_id sw_id x_limit

	# "\na_I3_HV6000_p331_allChannels_allSum"
	# "\nb_I3_HV6000_p331_allChannels_allSum"
	# "\nc_I3_HV6000_p331_allChannels_allSum"
	# "\nd_I3_HV6000_p331_allChannels_allSum"
	# "\ne_I3_HV6000_p331_allChannels_allSum"
	# "\nf_I3_HV6000_p331_allChannels_allSum"
	# "\ng_I3_HV6000_p331_allChannels_allSum"
	# "\nh_I3_HV6000_p331_allChannels_allSum"

	# #// WOM-C/-D,40Array0 sw1
	# "\n42_40SiPM_1_sw1_Calib_lOff_Int600_HV60_TestbeamConfig_240119 42 3075PE WOM-C sw1 3.5"
	# "\n50_40SiPM_2_sw1_Calib_lOff_Int600_HV60_TestbeamConfig_040219 50 3075PE WOM-D sw1 2.5"
	# "\n81_40SiPM_0_sw1_Calib_lOn_Int600_HV60_TestbeamConfig_210319 81 3075PE 40-Array0 sw1 3.0"

	# # // WOM-C/-D,40Array0 sw2
	# "\n41_40SiPM_1_sw2_Calib_lOff_Int590_HV60_TestbeamConfig_240119 41 3075PE WOM-C sw2 3.5"
	# "\n47_40SiPM_2_sw2_Calib_lOff_Int574_HV60_TestbeamConfig_040219 47 3075PE WOM-D sw2 2.5"
	# "\n77_40SiPM_0_sw2_Calib_lOn_Int590_HV60_TestbeamConfig_210319 77 3075PE 40-Array0 sw2 2.5"

	# # // WOM-C/-D,40Array0 sw4	
	# "\n73_40SiPM_0_sw4_Calib_lOn_Int590_HV60_TestbeamConfig_210319 73 3075PE 40-Array0 sw4 2.5"
	# "\n84_40SiPM_1_sw4_Calib_lOn_Int590_HV60_TestbeamConfig_210319 84 3075PE WOM-C sw4 3.0"
	# "\n88_40SiPM_2_sw4_Calib_lOn_Int580_HV60_TestbeamConfig_210319 88 3075PE WOM-D sw4 2.5"

	# # // WOM-A/-B,8Array0
	# "\n65_8SiPM_0_Calib_lOn_Int550_HV60_TestbeamConfig_210319 65 6075PE 8-Array0 none 4.5"
	# "\n68_8SiPM_1_Calib_lOn_Int550_HV60_TestbeamConfig_210319 68 6075PE WOM-A none 4.5"
	# "\n69_8SiPM_2_Calib_lOn_Int550_HV60_TestbeamConfig_210319 69 6075PE WOM-B none 4.0"		

	## // 8ArrayTB17
	# "\n82_8SiPM_17_Calib_lOn_Int550_HV60_TestbeamConfig_210319 82 6050PE 8-ArrayTB17 none 2."

	)

#echo -e ${data_list[@]} | tr '\n' '\0' | xargs -0 -n 1 -P 1 bash -c "do_average"



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
	echo $0; echo $1; echo $2; echo $3;
}
export -f test_fn2

# echo -e ${data_list[@]} | tr '\n' '\0' | xargs -0 -n 1 -P 1 bash -c "test_fn"