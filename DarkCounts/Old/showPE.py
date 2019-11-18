import numpy as np
import matplotlib.pyplot as plt

import pandas as pd
import uproot
import os

from os import listdir
from os.path import isfile, join
from PyPDF2 import PdfFileMerger
################
## INITIALIZE ##
################

# __ DARK COUNT _____________

# runName="35_40SiPM_1_sw4_DarkCount_lOff_HV60_TestbeamConfig_100119"
# runName="36_40SiPM_2_sw4_DarkCount_lOff_HV60_TestbeamConfig_100119"
# runName="40_40SiPM_1_sw2_DarCount_lOff_HV60_TestbeamConfig_240119"
# runName="43_40SiPM_1_sw1_DarCount_lOff_HV60_TestbeamConfig_240119"
# runName="48_40SiPM_2_sw2_DarkCount_lOff_HV60_TestbeamConfig_040219"
# runName="49_40SiPM_2_sw1_DarkCount_lOff_HV60_TestbeamConfig_040219"

runName="Dark Counts 07.05.2019"
# runName="67_8SiPM_1_DarkCount_lOff_HV60_TestbeamConfig_210319"
# runName="70_8SiPM_2_DarkCount_lOff_HV60_TestbeamConfig_210319"

# runName="74_40SiPM_0_DarkCount_lOff_HV60_TestbeamConfig_210319"
# runName="78_40SiPM_0_sw2_DarkCount_lOff_HV60_TestbeamConfig_210319"
# runName="80_40SiPM_0_sw1_DarkCount_lOff_HV60_TestbeamConfig_210319"
# runName="83_8SiPM_17_DarkCount_lOff_HV60_TestbeamConfig_210319"
# runName="86_40SiPM_1_DarkCount_lOff_HV60_TestbeamConfig_210319"
# runName="86_40SiPM_2_sw4_DarkCount_lOff_HV60_TestbeamConfig_210319"

# __ CALIB __________________

# runName="38_40SiPM_1_sw4_Calib_lOff_Int554_HV60_TestbeamConfig_240119"
# runName="41_40SiPM_1_sw2_Calib_lOff_Int590_HV60_TestbeamConfig_240119"
# runName="42_40SiPM_1_sw1_Calib_lOff_Int600_HV60_TestbeamConfig_240119"
# runName="44_40SiPM_2_sw4_Calib_lOff_Int554_HV60_TestbeamConfig_240119"
# runName="47_40SiPM_2_sw2_Calib_lOff_Int574_HV60_TestbeamConfig_040219"
# runName="50_40SiPM_2_sw1_Calib_lOff_Int600_HV60_TestbeamConfig_040219"

# runName="64_8SiPM_0_Calib_lOn_Int544_HV60_TestbeamConfig_210319"
# runName="65_8SiPM_0_Calib_lOn_Int536_HV60_TestbeamConfig_210319"
# runName="65_8SiPM_0_Calib_lOn_Int550_HV60_TestbeamConfig_210319"
# runName="68_8SiPM_1_Calib_lOn_Int544_HV60_TestbeamConfig_210319"
# runName="68_8SiPM_1_Calib_lOn_Int550_HV60_TestbeamConfig_210319"
# runName="69_8SiPM_2_Calib_lOn_Int550_HV60_TestbeamConfig_210319"

# runName="73_40SiPM_0_sw4_Calib_lOn_Int590_HV60_TestbeamConfig_210319"
# runName="75_40SiPM_0_sw4_Calib_lOn_Int570_HV60_TestbeamConfig_210319"
# runName="76_40SiPM_0_sw2_Calib_lOn_Int570_HV60_TestbeamConfig_210319"
# runName="77_40SiPM_0_sw2_Calib_lOn_Int590_HV60_TestbeamConfig_210319"
# runName="79_40SiPM_0_sw1_Calib_lOn_Int590_HV60_TestbeamConfig_210319"
# runName="81_40SiPM_0_sw1_Calib_lOn_Int600_HV60_TestbeamConfig_210319"
# runName="82_8SiPM_17_Calib_lOn_Int550_HV60_TestbeamConfig_210319"
# runName="84_40SiPM_1_sw4_Calib_lOn_Int590_HV60_TestbeamConfig_210319"
# runName="85_40SiPM_1_sw4_Calib_lOn_Int570_HV60_TestbeamConfig_210319"
# runName="87_40SiPM_2_sw4_Calib_lOn_Int570_HV60_TestbeamConfig_210319"
# runName="88_40SiPM_2_sw4_Calib_lOn_Int580_HV60_TestbeamConfig_210319"

# runName="a_I3_HV6000_p331_allChannels_allSum"
# runName="b_I3_HV6000_p331_allChannels_allSum"
# runName="c_I3_HV6000_p331_allChannels_allSum"
# runName="d_I3_HV6000_p331_allChannels_allSum"
# runName="e_I3_HV6000_p331_allChannels_allSum"
# runName="f_I3_HV6000_p331_allChannels_allSum"
# runName="g_I3_HV6000_p331_allChannels_allSum"
# runName="h_I3_HV6000_p331_allChannels_allSum"

runs = [
	#"dc_vb54",
	 #"dc_vb55",
	# "dc_vb56",
	 "10_calib_vb59_tune8630_pcbb",
	 "dc_vb58",
	 "dc_vb591",
 	"dc_vb60",
	 "dc_vb61",
	 "dc_vb62"
	] 

#tree = uproot.open("./a_root_files/0_constBL_Calib_ampW50_intW25_scale0/{0}.root".format(runName))["T"]
dirpath = os.getcwd()
print("current directory is : " + dirpath)
path = "E:\\Messungen\\Skripte\\Auswertung\\DarkCounts\\"
os.chdir( path )



for runID in runs:
	runName="./rootfiles/"+runID+".root"
	tree = uproot.open(runName)["T"]

	is_amp = 1
	if is_amp:
		# variable = "amp"
		# variable = "amp_inRange"
		variable = "chPE_amp"	
		variable2 = "amp_array"
	else:
		variable = "Integral_inRange"
		# variable = "Integral"
		# variable = "chPE_charge"
		variable2 = "charge_array"

	df_temp = tree.pandas.df([variable],flatten=False)



	df_ch = pd.DataFrame( df_temp[variable].values.tolist(), index=df_temp.index )
	del df_temp

	df_sum = tree.pandas.df([variable2])


	clmns = []
	for i in range(0,8):
			clmns.append("{1}_ch{0}".format(i,variable))

	df_ch = df_ch.drop([8],axis=1)
	df_ch.columns = clmns

	# df_sum.columns = "amp_array_A"
	# print(df_sum.head(10))

	###########
	## PLOTS ##
	###########

	nBins = 200
	x_min = -2
	sum_lim = 25
	ch_lim = 20

	fig0, ax0 = plt.subplots( nrows=3, ncols=3, figsize=(10,8) )
	fig0.suptitle("PE distribution of indiv. channels\nrun: {0}".format(runName))

	ax_chB = []
	for i in range(0,3):
		for k in range(0,3):
			ax_chB.append(ax0[i,k])

	for i in range(0,8):
		ax = ax_chB[i]
		ax.hist(df_ch[clmns[i]][df_ch[clmns[i]]<ch_lim].dropna().values, bins=nBins, histtype="stepfilled", log=False,label="{:s}\nmean = {:1.2f}".format(clmns[i],df_ch[clmns[i]].mean()))
		ax.legend(loc="best")
	#	ax.set_xticks((np.arange(x_min, ch_lim+1,1)))
		ax.set_xlim(x_min,ch_lim)
		ax.set_xlabel("$N_{p.e.}$")
		ax.set_ylabel("#Entries ")
		ax.set_yscale("log")


	ax = ax_chB[8]
	ax.hist(df_sum[variable2][df_sum[variable2]<sum_lim].dropna().values, bins=nBins, histtype="stepfilled", log=False,label="{:s}\nmean = {:1.2f}".format(variable2,df_sum[variable2].mean()),color="r")

	#ax.set_xticks((np.arange(x_min, sum_lim+1,2)))
	ax.set_xlim(x_min,sum_lim)
	ax.set_xlabel("$N_{p.e.}$")
	ax.set_ylabel("#Entries")
	# ax.set_yscale("log")
	ax.legend(loc="best")


	plt.subplots_adjust(left=0.08, right=0.97, top=0.9, bottom=0.06, hspace=0.28, wspace=0.23)
	plt.savefig("./Plots/"+runID+".pdf")
	plt.close()
	#plt.show()

onlyfiles = [f for f in listdir("./Plots/") if isfile(join("./Plots/", f))]
pdf_merger = PdfFileMerger()
for path in onlyfiles:
        pdf_merger.append("./Plots/"+path)
with open("./Plots/Combined_DC.pdf", 'wb') as fileobj:
        pdf_merger.write(fileobj)
 
