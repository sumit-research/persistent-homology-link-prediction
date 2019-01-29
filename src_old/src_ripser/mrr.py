import sys
import os
import struct
import pandas as pd
import numpy as np
from collections import OrderedDict
from subprocess import Popen, PIPE

def appendCSV(result, sep, out_file):
	if(os.path.isfile(out_file)):
		result.to_csv(out_file, mode = 'a', header = False, index = False)
	else:
		result.to_csv(out_file, sep = sep, index=False)

def mrr(data):
	columns = list(data.columns.values)
	temp_list = []

	temp_list.append("MRR")
	temp_list.append("-")

	for each in columns[2:]:
		data_l = list(data[each])
		data_l = [1/n for n in data_l]
		temp_list.append(sum(data_l)/float(len(data_l)))
	
	return temp_list

def map(data, position):
	columns = list(data.columns.values)
	temp_list = []

	temp_list.append("MAP")
	temp_list.append(position)

	for each in columns[2:]:
		data_l = data[each]
		temp_list.append(len(data_l[data_l <= position]))

	return temp_list


def main():
	if(len(sys.argv) != 2):
		print("[Usage:] python3 mrr.py csv_file")

	file = sys.argv[1]

	df = pd.read_csv(file)

	ex_data = []

	ex_data.append(mrr(df))
	

	for i in [1, 2, 3, 4, 5]:
		ex_data.append(map(df, i))

	new_df = pd.DataFrame(ex_data, columns = list(df.columns.values))
	appendCSV(new_df, ',', file)


if __name__ == '__main__':
	main()