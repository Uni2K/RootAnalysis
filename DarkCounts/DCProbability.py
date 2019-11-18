import glob, os, sys
import numpy as np
# import ROOT as r
# from multiprocessing import Pool
import pandas as pd
import matplotlib.pyplot as plt
import uproot

from os import listdir
from os.path import isfile, join

####################
## PE SLICES PROB ##
####################

def pe_slice_prob(df,pe_val):

	#__ DATAFRAME OF PE SLICES ___

	slice_names = []
	for i in pe_val:
		slice_names.append("N_pe{0}".format(i))	

	sc = 0 # slice counter
	d_pe_collection = {}
	# sum
	for k in pe_val:
		if k == 0:
			d_pe_collection[slice_names[sc]] = df.iloc[:,0][(df.iloc[:,0]<k+0.5) ]
		else:
			d_pe_collection[slice_names[sc]] = df.iloc[:,0][ (df.iloc[:,0]>=k-0.5) & (df.iloc[:,0]<k+0.5) ]
		sc = sc+1

	df_pe_slices = pd.DataFrame.from_dict(d_pe_collection)

	slice_prob = np.zeros( (1,len(pe_val)) )
		# sum
	sc = 0		
	for i in range(0,len(pe_val)):
		slice_prob[0,i] = len( df_pe_slices[slice_names[sc]].dropna() ) / len(df)
		sc = sc+1

	df_slice_sum_prob = pd.DataFrame(data=slice_prob.T,columns=df.columns)
	
	return df_slice_sum_prob 

#################
## PE CUM PROB ##
#################

def pe_cum_prob(df_pe_slices_prob,pe_val):
	#__ DATAFRAME OF CUMMULATIVE PE PROBABILITIES ____
	slice_cprob = np.zeros( (1,len(pe_val)) )
	# sum
	sc = 0		
	for i in range(0,len(pe_val)):
		slice_cprob[0,i] = df_pe_slices_prob.iloc[i:len(pe_val),0].sum()
		sc = sc+1

	df_slice_cprob = pd.DataFrame(data=slice_cprob.T,columns=df_pe_slices_prob.columns)
	return df_slice_cprob

################
## INITIALIZE ##
################

directory = os.path.join( os.getcwd())

# source directory
root_dir = str(directory+"/../rootfiles/")

# output directories
out_dir = str(directory+"/dc_prob_results/charge/")
if not os.path.isdir(out_dir):
    os.system("mkdir -p %s"%out_dir)

if not os.path.isdir(str(out_dir+"3075PE")):
    os.system("mkdir -p %s"%str(out_dir+"3075PE"))
if not os.path.isdir(str(out_dir+"6075PE")):
    os.system("mkdir -p %s"%str(out_dir+"6075PE"))
if not os.path.isdir(str(out_dir+"6050PE")):
    os.system("mkdir -p %s"%str(out_dir+"6050PE"))


# ___ INPUT LISTs ___


run_list = [f for f in listdir(root_dir) if isfile(join(root_dir, f))]


# data frame column names
clmn_list = [
	"40SiPM_1_sw1",
	"40SiPM_2_sw1"
]
# plot title info
label_list = [
	"WOM-C, sw1",
	"WOM-D, sw1"
]
# sipm type
sipm_id_list = [
	"3075PE",
	"3075PE"
]
# wom/array type
wom_id_list = [
	"WOM-C",
	"WOM-D"
]
# switch configuration
sw_id_list = [
	"sw1",
	"sw1"
]

#############
## ANALYZE ##
#############

# create N_pe threshold array
npe_max = 30
pe_val = np.zeros(npe_max+1)
for i in range(0,npe_max+1):
	pe_val[i] = i

# store DC spectra
d_data = {}

# store prob results
d_prob = {}
d_cprob = {}
v_mean = []

#__ loop over ALL RUNS ____

run_count = 0
for runName in run_list:
	print(runName)
	tree = uproot.open(root_dir+"/{0}".format(runName))["T"]

	ch_variable = "Integral"
	sum_variable = "Amplitude"
	# variable3 = "charge_array_scaled"

	# get single channel charge branch
	df_temp = tree.pandas.df([ch_variable],flatten=False)
	df_ch = pd.DataFrame( df_temp[ch_variable].values.tolist(), index=df_temp.index )
	del df_temp

	clmns = []
	for i in range(0,8):
			clmns.append("{1}_ch{0}".format(i,ch_variable))

	df_ch = df_ch.drop([0],axis=1)
	df_ch.columns = clmns
	df_ch = df_ch.replace([np.inf, -np.inf], np.nan)

	# get sum branch
	df_sum = tree.pandas.df([sum_variable])
	df_sum = pd.DataFrame(df_sum.iloc[:,0],columns=[sum_variable])
	df_sum = df_sum.replace([np.inf, -np.inf], np.nan)

	# loop over datasets, 8 channels + 1 sum
	for i in range(0,9):
		
		df_charge = pd.DataFrame()
		data_id = ""
		if i!=8:
			df_charge = pd.DataFrame(df_ch.iloc[:,i],columns=[clmns[i]])
			data_id = "{:s}_ch{:d}".format(clmn_list[run_count],i)
		else:
			df_charge = df_sum
			data_id = "{:s}_sum".format(clmn_list[run_count])

		# mean of distribution
		mean_charge = df_charge.mean()

		# probability of PE slices 
		df_pe_slices_prob = pe_slice_prob(df_charge,pe_val)	

		# print("PE SLICES PROB")
		# print(df_pe_slices_prob.head(10))
		# print(df_pe_slices_prob.sum(axis=0))

		# cumulative probability of PE slices 
		df_pe_slice_cprob = pe_cum_prob(df_pe_slices_prob,pe_val)

		# print("CUMM. PROB")	
		# print(df_pe_slice_cprob)

		#___ SAVE RESULTS ____
		d_data["{:s}".format(data_id)] = df_charge.iloc[:,0]
		d_prob["prob_{:s}".format(data_id)] = df_pe_slices_prob.iloc[:,0]
		d_cprob["cprob_{:s}".format(data_id)] = df_pe_slice_cprob.iloc[:,0]
		v_mean.append( mean_charge )

	run_count = run_count + 1
print(d_data)

df_data = pd.DataFrame.from_dict(d_data)

df_pe_val = pd.DataFrame(data=pe_val,columns=["Thr"])
df_prob = pd.DataFrame.from_dict(d_prob)
df_prob = pd.concat(([df_pe_val,df_prob]),axis=1)
df_cprob = pd.DataFrame.from_dict(d_cprob)
df_cprob = pd.concat(([df_pe_val,df_cprob]),axis=1)

df_mean = pd.DataFrame(np.array(v_mean))

# print(df_cprob)
# print(df_mean)

###########
## PLOTS ##
###########


# create set of bin-defining histogram edges of variable size
# for 1-bin-per-Npe plot
x_min = -6.5
xbins = np.zeros(npe_max+2)
xbins[0] = x_min; xbins[1] = 0.5
for i in range(1,npe_max+1):
	xbins[i+1] = i+0.5

x_max = 30.5

ch_prob_color = "black"
ch_hist_color = "dodgerblue"
sum_prob_color = "black"
sum_hist_color = "red"
y_labelsize = 8
x_labelsize = 8
grid_alpha = 0.15
leg_fsize = 9

ch_npe_lim = 1

# loop over runs
for i in range(0,len(run_list)):
	
	#_____ SINGLE PLOTS ____
	# loop over datasets, 8 channels + 1 sum

	if sw_id_list[i]=="none":
		fig_title = "dark count pulse-height spectrum - Hamamatsu S13360-{:s}, {:s} ".format(sipm_id_list[i], wom_id_list[i])
	else:
		fig_title = "dark count pulse-height spectrum - Hamamatsu S13360-{:s}, {:s}, {:s} ".format(sipm_id_list[i], wom_id_list[i], sw_id_list[i])

	for k in range(0,9):
		
		data_id = ""
		single_fig_title = ""
		pdf_filename = ""
		if k!=8:
			data_id = "{:s}_ch{:d}".format(clmn_list[i],k)
			single_fig_title = str(fig_title+", ch{:d}".format(k))
			pdf_filename = "{:s}{:s}/{:s}_{:s}_ch{:d}.pdf".format(out_dir,sipm_id_list[i],run_list[i],wom_id_list[i],k)
			x_min = -1.5
			x_max = 20.5
			x_ticks = np.arange(0, x_max+1,1)
		else:
			data_id = "{:s}_sum".format(clmn_list[i])
			single_fig_title = str(fig_title+", sum")
			pdf_filename = "{:s}{:s}/{:s}_{:s}_sum.pdf".format(out_dir,sipm_id_list[i],run_list[i],wom_id_list[i])
			x_min = -5.5
			x_max = 30.5
			x_ticks = np.arange(0, x_max+1,5)

		fig0, ax0 = plt.subplots( nrows=1, ncols=1, figsize=(6,5) )
		fig0.suptitle(single_fig_title,ha="left", x = 0.05, y = 0.96, fontsize=9)

		ax = ax0

		## weights normalize by number of histogram entries
		weights = np.ones_like(df_data[data_id].dropna().values)/float(len(df_data[data_id].dropna().values))

		## dc pulse-heigt, 1 bin per photoelctron
		ax.hist(df_data[data_id].dropna().values, weights=weights, bins=xbins, histtype="step",color=ch_hist_color,alpha=0.8, log=True,label="dc pulse-height dist.\n1 bin per photoelectron\nmean: $N_{{pe}}$ = {:1.2f}\nentries: {:d} ".format(df_mean.iloc[(k+i*9),0],len(df_data[data_id].dropna().values)),zorder=2,density=False)

		## dc pulse-heigt, 10 bin per photoelctron
		nBins = int(((df_data[data_id].max() - df_data[data_id].min()) *10).round(0))
		ax.hist(df_data[data_id].dropna().values, weights=weights, bins=nBins, histtype="stepfilled",color=sum_hist_color,alpha=0.8, log=True,label="dc pulse-height dist.\n10 bin per photoelectron\nmean: $N_{{pe}}$ = {:1.2f}\nentries: {:d} ".format(df_mean.iloc[(k+i*9),0],len(df_data[data_id].dropna().values)),zorder=1,density=False)

		## probability, N_pe-wise
		# ax.plot(pe_val,df_prob.iloc[:,(k+i*9)+1].values,color=ch_prob_color,linestyle="-",linewidth=1,ms=3,marker="s",label="dark count prob.\n$P(N_{{pe}}={:d})$ = {:1.1f} %".format(ch_npe_lim,df_prob.iloc[ch_npe_lim,(k+i*9)+1]*100),zorder=2)
		## probability, cumulative
		ax.plot(pe_val,df_cprob.iloc[:,(k+i*9)+1].values,color=ch_prob_color,linestyle=":",linewidth=1,ms=3,marker="o",label=("dark count cum. prob.\n"+r"$P^{{\ast}}(N_{{pe}}{{\geq}}{:d})$ = {:1.1f} %".format(ch_npe_lim,df_cprob.iloc[ch_npe_lim,(k+i*9)+1]*100)),zorder=2)

		# ax.set_title(single_fig_title,fontsize=10)
		ax.set_xlabel("number-of-photoelectrons [$N_{pe}$]",fontsize=9)
		ax.set_ylabel("probability",fontsize=9)
		ax.legend(loc="best",fontsize=8)
		ax.set_xticks(x_ticks)
		ax.set_xlim(x_min,x_max)
		# ax.tick_params(axis='y', labelcolor=ch_hist_color,labelsize=y_labelsize)
		ax.grid(True,"both","both",alpha=grid_alpha,color=ch_prob_color)

		plt.subplots_adjust(left=0.12, right=0.99, top=0.9, bottom=0.1, hspace=0.15, wspace=0.2)
		plt.savefig(pdf_filename)
		plt.close()	


	#_____ COMBINED PLOT ____
	fig1, ax1 = plt.subplots( nrows=3, ncols=3, figsize=(11,10) )

	fig1.suptitle(fig_title)
	ax_comb = []
	for j in range(0,3):
		for m in range(0,3):
			ax_comb.append(ax1[j,m])

	comb_pdf_filename = "{:s}{:s}/{:s}_{:s}.pdf".format(out_dir,sipm_id_list[i],run_list[i],wom_id_list[i])

	# loop over datasets, 8 channels + 1 sum
	for k in range(0,9):
		
		data_id = ""
		single_fig_title = ""
		if k!=8:
			data_id = "{:s}_ch{:d}".format(clmn_list[i],k)
			single_fig_title = "ch{:d}".format(k)
			x_min = -1.5
			x_max = 20.5
			x_ticks = np.arange(0, x_max+1,2)
		else:
			data_id = "{:s}_sum".format(clmn_list[i])			
			single_fig_title = "sum"
			x_min = -5.5
			x_max = 30.5
			x_ticks = np.arange(0, x_max+1,5)

		ax = ax_comb[k]

		## weights normalize by number of histogram entries
		weights = np.ones_like(df_data[data_id].dropna().values)/float(len(df_data[data_id].dropna().values))

		## dc pulse-heigt, 1 bin per photoelctron
		ax.hist(df_data[data_id].dropna().values, weights=weights, bins=xbins, histtype="step",color=ch_hist_color,alpha=0.8, log=True,label="dc pulse-height dist.\n1 bin per photoelectron\nmean: $N_{{pe}}$ = {:1.2f}\nentries: {:d} ".format(df_mean.iloc[(k+i*9),0],len(df_data[data_id].dropna().values)),zorder=2,density=False)

		## dc pulse-heigt, 10 bin per photoelctron
		nBins = int(((df_data[data_id].max() - df_data[data_id].min()) *10).round(0))
		ax.hist(df_data[data_id].dropna().values, weights=weights, bins=nBins, histtype="stepfilled",color=sum_hist_color,alpha=0.8, log=True,label="dc pulse-height dist.\n10 bin per photoelectron\nmean: $N_{{pe}}$ = {:1.2f}\nentries: {:d} ".format(df_mean.iloc[(k+i*9),0],len(df_data[data_id].dropna().values)),zorder=1,density=False)

		## probability, N_pe-wise
		# ax.plot(pe_val,df_prob.iloc[:,(k+i*9)+1].values,color=ch_prob_color,linestyle="-",linewidth=1,ms=3,marker="s",label="dark count prob.\n$P(N_{{pe}}={:d})$ = {:1.1f} %".format(ch_npe_lim,df_prob.iloc[ch_npe_lim,(k+i*9)+1]*100),zorder=2)
		
		## probability, cumulative
		ax.plot(pe_val,df_cprob.iloc[:,(k+i*9)+1].values,color=ch_prob_color,linestyle=":",linewidth=1,ms=3,marker="o",label=("dark count cum. prob.\n"+r"$P^{{\ast}}(N_{{pe}}{{\geq}}{:d})$ = {:1.1f} %".format(ch_npe_lim,df_cprob.iloc[ch_npe_lim,(k+i*9)+1]*100)),zorder=2)

		ax.set_title(single_fig_title,fontsize=10)
		ax.set_xlabel("number-of-photoelectrons [$N_{pe}$]",fontsize = 9)
		ax.set_ylabel("probability",fontsize = 9)
		ax.legend(loc="best",fontsize=6.5)
		ax.set_xticks(x_ticks)
		ax.set_xlim(x_min,x_max)
		# ax.tick_params(axis='y', labelcolor=ch_hist_color,labelsize=y_labelsize)
		ax.grid(True,"both","both",alpha=grid_alpha,color=ch_prob_color)

	plt.subplots_adjust(left=0.06, right=0.99, top=0.93, bottom=0.06, hspace=0.35, wspace=0.30)
	plt.savefig(comb_pdf_filename)
	plt.close()	

##___ COMBINED SUM PLOTS ____

fig2, ax2 = plt.subplots( nrows=1, ncols=1, figsize=(6,6) )
fig_title = "dark count - cumulative probability, Hamamatsu S13360 series"
fig2.suptitle(fig_title)
ax = ax2

comb_sum_pdf_filename = "{:s}/cprob_all_charge.pdf".format(out_dir)

m_size = 3.5
m_style = ["D", "v", "^","*","d", "o", "s"]
l_style = ["--", "-", "-.",":","--", "-", "-."]
l_color = ["mediumvioletred", "darkgreen", "darkblue", "red","darkorange", "mediumseagreen", "deepskyblue"]
# select only cases used in test-beams
select_runs = [6,7,8,9,10,11,12]
select_cntr = 0
for i in select_runs:
	## probability, cumulative
	leg_label = str("{:s}, {:s}\n".format(sipm_id_list[i],wom_id_list[i])+r"$P^{{\ast}}(N_{{pe}}{{\geq}}{:d})$ = {:1.1f} %".format(ch_npe_lim,df_cprob.iloc[ch_npe_lim,(8+i*9)+1]*100))
	ax.plot(pe_val,df_cprob.iloc[:,(8+i*9)+1].values,color=l_color[select_cntr],linestyle=l_style[select_cntr],linewidth=1,ms=m_size,marker=m_style[select_cntr],label=leg_label ,zorder=2)
	select_cntr += 1

	ax.set_yscale("log")
	ax.set_xlabel("number-of-photoelectrons [$N_{pe}$]",fontsize = 9)
	ax.set_ylabel("probability",fontsize = 9)
	ax.legend(bbox_to_anchor=(0.00, -0.375), loc="lower left", ncol=3,fontsize=8)
	ax.grid(True,"both","both",alpha=grid_alpha,color=ch_prob_color)
	


plt.subplots_adjust(left=0.12, right=0.99, top=0.93, bottom=0.25, hspace=0.35, wspace=0.30)
plt.savefig(comb_sum_pdf_filename)
plt.close()

##___ 40SIPM Arrays COMBINED SUM PLOTS ____

fig3, ax3 = plt.subplots( nrows=1, ncols=1, figsize=(6,6) )
fig_title = "dark count - cumulative probability, Hamamatsu S13360-3075PE"
fig3.suptitle(fig_title)
ax = ax3

comb_40SiPM_sum_pdf_filename = "{:s}/cprob_40SiPM_charge.pdf".format(out_dir)

m_size = 3.5
m_style = ["D", "v", "^","D", "v", "^","D", "v", "^",]
l_style = ["--", "-", "-.","--", "-", "-.","--", "-", "-."]
l_width = [0.6,0.6,0.6,1,1,1,1.5,1.5,1.5]
l_alpha = [1,1,1,0.7,0.7,0.7,0.4,0.4,0.4]
l_zorder = [1,1,1,2,2,2,3,3,3]
# l_color = ["mediumvioletred", "darkgreen", "darkblue","mediumvioletred", "darkgreen", "darkblue","mediumvioletred", "darkgreen", "darkblue"]
l_color = ["darkblue", "darkblue", "darkblue", "darkgreen","darkgreen","darkgreen","mediumvioletred","mediumvioletred","mediumvioletred" ]

# select only 40 SiPM arrays
# select_runs = [0,1,2,3,4,5,6,7,8]
select_runs = [6,7,8,5,3,4,2,0,1]
# select_runs = [2,5,6,0,3,7,1,4,8]
select_cntr = 0
for i in select_runs:
	## probability, cumulative
	leg_label = str("{:s}\n".format(label_list[i])+r"$P^{{\ast}}(N_{{pe}}{{\geq}}{:d})$ = {:1.1f} %".format(ch_npe_lim,df_cprob.iloc[ch_npe_lim,(8+i*9)+1]*100))
	ax.plot(pe_val,df_cprob.iloc[:,(8+i*9)+1].values,color=l_color[select_cntr],alpha=l_alpha[select_cntr],linestyle=l_style[select_cntr],linewidth=l_width[select_cntr],ms=m_size,marker=m_style[select_cntr],label=leg_label ,zorder=l_zorder[select_cntr])
	select_cntr += 1

	ax.set_yscale("log")
	ax.set_xlabel("number-of-photoelectrons [$N_{pe}$]",fontsize = 9)
	ax.set_ylabel("probability",fontsize = 9)
	ax.legend(bbox_to_anchor=(0.00, -0.375), loc="lower left", ncol=3,fontsize=8)
	ax.grid(True,"both","both",alpha=grid_alpha,color=ch_prob_color)
	


plt.subplots_adjust(left=0.12, right=0.99, top=0.93, bottom=0.25, hspace=0.35, wspace=0.30)
plt.savefig(comb_40SiPM_sum_pdf_filename)
plt.close()


