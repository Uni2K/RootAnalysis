from math import sqrt
import matplotlib.pyplot as plt
import seaborn as sns
sns.set_style("ticks")
pe = '12'
particle = 'muons'


#eff2017 muons, Npe>8
dist_2017 = [22.6, 35.8, 16, 16, 35.8, 32, 0] #0 1 2 3 4 5 7
eff_2017 = [99.9344, 88.0952, 99.9771, 100, 91.5213, 95.8685, 100] #muons
err_low_2017 = [0.03, 0.3, 0.03, 0.02, 0.03, 0.2, 0.07] #muons
err_up_2017 = [0.03, 0.3, 0.02, 0, 0.3, 0.2, 0] #muons
eff_2017 = [a/100 for a in eff_2017]
err_low_2017 = [a/100 for a in err_low_2017]
err_up_2017 = [a/100 for a in err_up_2017]

#dict with coordinates on box
positions = {'1': [40,104], '2': [20,89.9], '3': [60,89.9], '4': [10,60], '5': [40,60], '6': [70,60], '7': [20,30], '8': [60,30], '9': [40,10], '10': [50,74.9], '11': [30,45], '12': [10,15], '13': [70,15], '14': [50,45], '15': [30,75], '16': [69,104]}

#read in txt files for all WOMs
f_A = open('./efficiency_vs_position_14_WOMA_'+particle+'_int.txt','r')
pos_A = []
eff_A = []
err_low_A = []
err_up_A = []
dist_A = []
for line in f_A:
	line = line.split()
	pos_A.append(int(line[0]))
	eff_A.append(float(line[1]))
	err_low_A.append(float(line[2]))
	err_up_A.append(float(line[3]))
	dist_A.append(sqrt(((80-9.2)-positions[line[0]][0])**2+((120-9.2)-positions[line[0]][1])**2))
f_A.close()

f_B = open('./efficiency_vs_position_14_WOMB_'+particle+'_int.txt','r')
pos_B = []
eff_B = []
err_low_B = []
err_up_B = []
dist_B = []
for line in f_B:
	line = line.split()
	pos_B.append(int(line[0]))
	eff_B.append(float(line[1]))
	err_low_B.append(float(line[2]))
	err_up_B.append(float(line[3]))
	dist_B.append(sqrt((positions[line[0]][0]-9.2)**2+(positions[line[0]][1]-9.2)**2))
f_B.close()

f_C = open('./efficiency_vs_position_13_WOMC_'+particle+'_int.txt','r')
pos_C = []
eff_C = []
err_low_C = []
err_up_C = []
dist_C = []
for line in f_C:
	line = line.split()
	pos_C.append(int(line[0]))
	eff_C.append(float(line[1]))
	err_low_C.append(float(line[2]))
	err_up_C.append(float(line[3]))
	dist_C.append(sqrt((positions[line[0]][0]-9.2)**2+((120-9.2)-positions[line[0]][1])**2))
f_C.close()

f_D = open('./efficiency_vs_position_13_WOMD_'+particle+'_int.txt','r')
pos_D = []
eff_D = []
err_low_D = []
err_up_D = []
dist_D = []
for line in f_D:
	line = line.split()
	pos_D.append(int(line[0]))
	eff_D.append(float(line[1]))
	err_low_D.append(float(line[2]))
	err_up_D.append(float(line[3]))
	dist_D.append(sqrt(((80-9.2)-positions[line[0]][0])**2+(positions[line[0]][1]-9.2)**2))
f_D.close()


#create plot
fig, ax1 = plt.subplots(figsize=(8,4))
left, bottom, width, height = [0.20, 0.2, 0.3, 0.3]
ax2 = fig.add_axes([left, bottom, width, height])

ax1.errorbar(dist_A,eff_A,yerr=[err_low_A,err_up_A],marker='o',linestyle='', capsize=3,ecolor='#1f77b4',color='#1f77b4', label='2018 WOM A')
ax1.errorbar(dist_B,eff_B,yerr=[err_low_B,err_up_B],marker='v',linestyle='', capsize=3,ecolor='#ff7f0e',color='#ff7f0e', label='2018 WOM B')
ax1.errorbar(dist_C,eff_C,yerr=[err_low_C,err_up_C],marker='s',linestyle='', capsize=3,ecolor='#2ca02c',color='#2ca02c', label='2018 WOM C')
ax1.errorbar(dist_D,eff_D,yerr=[err_low_D,err_up_D],marker='d',linestyle='', capsize=3,ecolor='#d62728',color='#d62728', label='2018 WOM D')

ax1.grid(zorder=0)
ax1.set_xlabel('Distance to WOM-Center [cm]', fontsize=10)
ax1.set_ylabel('Efficiency,   N$_{p.e.}^{A,B}\geq$ 14 , N$_{p.e.}^{C,D}\geq$ 13', fontsize=10)
ax1.legend(loc='lower left', bbox_to_anchor=(0.086, 0.55))
# ax1.legend(loc='lower left')



#create plot-inset
def list_for_insert(dist, eff, err_low, err_up):
	dist_inset = [a for a in dist if a<50]
	eff_inset = [b for a,b in zip(dist,eff) if a<50]
	err_low_inset = [b for a,b in zip(dist,err_low) if a<50]
	err_up_inset = [b for a,b in zip(dist,err_up) if a<50]
	return dist_inset, eff_inset, err_low_inset, err_up_inset


dist_inset_A, eff_inset_A, err_low_inset_A, err_up_inset_A = list_for_insert(dist_A, eff_A, err_low_A, err_up_A)
dist_inset_B, eff_inset_B, err_low_inset_B, err_up_inset_B = list_for_insert(dist_B, eff_B, err_low_B, err_up_B)
dist_inset_C, eff_inset_C, err_low_inset_C, err_up_inset_C = list_for_insert(dist_C, eff_C, err_low_C, err_up_C)
dist_inset_D, eff_inset_D, err_low_inset_D, err_up_inset_D = list_for_insert(dist_D, eff_D, err_low_D, err_up_D)
dist_inset_2017, eff_inset_2017, err_low_inset_2017, err_up_inset_2017 = list_for_insert(dist_2017, eff_2017, err_low_2017, err_up_2017)

ax2.errorbar(dist_inset_A, eff_inset_A, yerr=[err_low_inset_A,err_up_inset_A],marker='o',linestyle='', capsize=3,ecolor='#1f77b4',color='#1f77b4')
ax2.errorbar(dist_inset_B, eff_inset_B, yerr=[err_low_inset_B,err_up_inset_B],marker='v',linestyle='', capsize=3,ecolor='#ff7f0e',color='#ff7f0e')
ax2.errorbar(dist_inset_C, eff_inset_C, yerr=[err_low_inset_C,err_up_inset_C],marker='s',linestyle='', capsize=3,ecolor='#2ca02c',color='#2ca02c')
ax2.errorbar(dist_inset_D, eff_inset_D, yerr=[err_low_inset_D,err_up_inset_D],marker='d',linestyle='', capsize=3,ecolor='#d62728',color='#d62728')
ax2.grid(zorder=0)

plt.savefig('./efficiency_vs_position_1413_allWOMs_'+particle+'_int.pdf')

