import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import uproot
import os
from os import listdir
from os.path import isfile, join
import re
import math
from scipy.optimize import curve_fit
import astropy
from astropy.modeling import models, fitting
from scipy.special import factorial

"""Get into correct Dir"""
dirpath = os.getcwd()
path = "E:\\Messungen\\Skripte\\TestStrahl\\rootfiles\\"   
os.chdir( path )
run="out.root"
tree = uproot.open(run)["T"]
variable="tSiPM"
df_temp = tree.pandas.df([variable],flatten=False)
df_ch = pd.DataFrame( df_temp[variable].to_numpy().tolist(), index=df_temp.index )
df_ch=df_ch[df_ch.replace([np.inf, -np.inf], np.nan).notnull().all(axis=1)]   #Drop "inf"
del df_temp

#FIT
channelStart=7
channelEnd=13
numberBins=200
nLeft=140
nRight=209 # To only fit the center
bins = np.linspace(-80, -10, 5000,dtype=int) #NOT USED
stdArray=[]
meanArray=[]

onlyCenter=0
fig, ax = plt.subplots(3, 3)
ax_ch = []
for i in range(0,3):
	for k in range(0,3):
		ax_ch.append(ax[i,k])

def gaussian(x, mean, amplitude, standard_deviation):
    return amplitude * np.exp( - ((x - mean) / standard_deviation) ** 2)

counter=0 #since we start not from 0
for i in range(channelStart,channelEnd+1):
    data=df_ch.iloc[:,i]
    data_entries_, bins_ = np.histogram(data, bins="auto")
    left,right = bins_[:-1],bins_[1:]
    X = np.array([left,right]).T.flatten()
    Y = np.array([data_entries_,data_entries_]).T.flatten()

    #FIT DATA, ONLY CENTER DATA
    XFit = np.array([left,right]).T.flatten().tolist()
    YFit = np.array([data_entries_,data_entries_]).T.flatten().tolist()

    if onlyCenter:
        del XFit[:nLeft]
        del YFit[:nLeft]
        del XFit[-nRight:]
        del YFit[-nRight:]


    ax = ax_ch[counter]
    ax.set_ylabel("$TIME$", fontsize=7)
    ax.set_xlabel("$V_{bias}$")
    ax.tick_params(axis='y', which='major', labelsize=5)
    ax.tick_params(axis='y', which='minor', labelsize=5)
    ax.tick_params(axis='x', which='major', labelsize=6)
    ax.tick_params(axis='x', which='minor', labelsize=7)
    ax.set_title("Channel: {} ".format(i),fontsize=10)

    ax.hist(data,bins=X, color="red")
    popt, pcov = curve_fit(gaussian, xdata=XFit, ydata=YFit,p0=[-55., 4000.,30.])
    print(popt)
    stdArray.append(popt[2])
    meanArray.append(popt[0])

    ax.plot(XFit, gaussian(XFit, *popt), color='green')
    counter=counter+1

#https://stackoverflow.com/questions/52591979/how-to-obtain-the-chi-squared-value-as-an-output-of-scipy-optimize-curve-fit
plt.show()






