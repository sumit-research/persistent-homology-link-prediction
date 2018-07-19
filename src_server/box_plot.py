import sys
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from collections import OrderedDict
import seaborn as sns

def main():
	if(len(sys.argv) != 2):
		print("[Usage:] python3 box_plot.py data_file")
		exit()

	data_file = sys.argv[1]
	df = pd.read_csv(data_file, sep=',')
	print(df.head(10))
	# for col in ["W_a_0", "B_a_0", "W_b_0", "B_b_0", "W_a_1", "B_a_1", "W_b_1", "B_b_1", "W_comp_0", "B_comp_0", "W_comp_1", "B_comp_1"]:
	# 	bplot = sns.lmplot(y=col, x="distance", 
 #                 data=df, 
 #                 palette="muted")
	# 	# bplot = sns.boxplot(y=col, x="distance", 
 #  #                data=df, 
 #  #                width=0.5,
 #  #                palette="muted")
	# 	# boxplot = df.boxplot(column = col, by = "distance")
	# 	plt.show()

	aa = list(df["AA"])
	aa = [a for a in aa if a > 0.0]
	new_df = OrderedDict()
	new_df["AA"] = aa
	new_df["W_a_0"] = df["W_a_0"]

	new_df = pd.DataFrame(new_df)

	bplot = sns.lmplot(y="AA", x="W_a_0", 
                 data=new_df, 
                 palette="muted")
	plt.show()


if __name__ == '__main__':
	main()
# W_a_0	B_a_0	W_b_0	B_b_0	W_a_1	B_a_1	W_b_1	B_b_1	W_comp_0	B_comp_0	W_comp_1	B_comp_1	AA	MW