import sys
import os
import struct
import pandas as pd
import numpy as np
from collections import OrderedDict
from subprocess import Popen, PIPE
# get input nodes and define neighbourhood
# input filename
# n-hop neighbourhood -- n

def appendCSV(final_results, sep):
	result = pd.DataFrame(final_results, columns = final_results[0].keys())
	if(os.path.isfile("./test_results.csv")):
		result.to_csv("./test_results.csv", mode = 'a', header = False, index = False)
	else:
		result.to_csv("./test_results.csv", sep = sep, index=False)

def main():
	if(len(sys.argv) != 3):
		print("[Usage:] python3 script.py data_file n")
		exit()

	data_file = sys.argv[1]
	hop = int(sys.argv[2])

	if(os.path.exists("/home/deepak/Project/files/outputs/n_" + str(hop)) == False):
		os.system("mkdir /home/deepak/Project/files/outputs/n_" + str(hop))

	# compile all c++ files


	# get all the reachable pairs in the graph to test

	os.system("/home/deepak/Project/code/src_server/johnson --dump_pairs " + data_file + " /home/deepak/Project/files/outputs/n_" + str(hop) + "/global.txt /home/deepak/Project/files/outputs/n_" + str(hop) + "/global_sparse.txt" )
	data = "/home/deepak/Project/files/outputs/dumped.txt"
	# data = "/home/deepak/Project/code/src_server/random_select.txt"

	# define a list of ordered dict to save the results in excel
	final_results = []
	lines = []
	total_pairs = 0

	# open the data file and repeat the process for each pair
	with open(data, "r") as f:
		lines = f.readlines()
		lines = [l.strip().split() for l in lines]
		
	for l in lines:
		node_a = int(l[1])
		node_b = int(l[2])
		temp = OrderedDict()
		temp["node_a"] = node_a
		temp["node_b"] = node_b
		temp["distance"] = int(l[0])
		total_pairs+=1

		# define file names for persistence diagrams

		dgm1_file = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/dipha_" + str(node_a)
		dgm2_file = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/dipha_" + str(node_b)
		dgmCombine_file = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/dipha_" + str(node_a) + "_" + str(node_b)
		dgmComplete_file = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/dipha_complete_" + str(node_a) + "_" + str(node_b)

		# obtain persistence diagrams for node_a, node_b and combined

		if(not os.path.isfile(dgm1_file)):
			os.system("python3 /home/deepak/Project/code/src_server/get_persDiag.py " + data_file + " " + str(node_a) + " " + str(hop))
		if(not os.path.isfile(dgm2_file)):
			os.system("python3 /home/deepak/Project/code/src_server/get_persDiag.py " + data_file + " " + str(node_b) + " " + str(hop))

		os.system("python3 /home/deepak/Project/code/src_server/get_persDiag.py " + data_file + " " + str(node_a) + " " + str(node_b) + " " + str(hop))

		# get persistence diagram for complete graph
		in_file = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/apsp_complete_full_" + str(node_a) + "_" + str(node_b)
		f = open(in_file, "rb")
		data = f.read()
		num_processors = struct.unpack('<q' , data[16:24])[0]
		if(num_processors > 12):
			num_processors = 12
		command = "mpiexec -n " + str(num_processors) + " dipha --upper_dim 2 " + in_file + " " + dgmComplete_file
		os.system(command)

		# compare pairwise diagrams
		process_a_b = Popen(["/home/deepak/Project/code/src_server/baseline", data_file, "/home/deepak/Project/code/src_server/test.txt", str(node_a), str(node_b)], stdout=PIPE)
		process_a_0 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgm1_file, str(2),str(0)], stdout=PIPE)
		process_a_1 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgm1_file, str(2),str(1)], stdout=PIPE)
		process_b_0 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgm2_file, str(2),str(0)], stdout=PIPE)
		process_b_1 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgm2_file, str(2),str(1)], stdout=PIPE)
		process_complete_0 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgmComplete_file, str(2),str(0)], stdout=PIPE)
		process_complete_1 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgmComplete_file, str(2),str(1)], stdout=PIPE)
		
		# print(node_a, node_b)
		(output_a_b,err) = process_a_b.communicate()
		output_a_b = output_a_b.strip().splitlines()

		(output_a_0,err) = process_a_0.communicate()
		output_a_0 = output_a_0.strip().splitlines()

		(output_a_1,err) = process_a_1.communicate()
		output_a_1 = output_a_1.strip().splitlines()

		(output_b_0,err) = process_b_0.communicate()
		output_b_0 = output_b_0.strip().splitlines()

		(output_b_1,err) = process_b_1.communicate()
		output_b_1 = output_b_1.strip().splitlines()

		(output_complete_0, err) = process_complete_0.communicate()
		output_complete_0 = output_complete_0.strip().split()

		(output_complete_1, err) = process_complete_1.communicate()
		output_complete_1 = output_complete_1.strip().split()

		# create a temporary dictionary
			
		temp["W_a_0"] = float(output_a_0[0])
		temp["B_a_0"] = float(output_a_0[1])

		temp["W_b_0"] = float(output_b_0[0])
		temp["B_b_0"] = float(output_b_0[1])

		temp["W_a_1"] = float(output_a_1[0])
		temp["B_a_1"] = float(output_a_1[1])

		temp["W_b_1"] = float(output_b_1[0])
		temp["B_b_1"] = float(output_b_1[1])

		temp["W_comp_0"] = float(output_complete_0[0])
		temp["B_comp_0"] = float(output_complete_0[1])

		temp["W_comp_1"] = float(output_complete_1[0])
		temp["B_comp_1"] = float(output_complete_1[1])
		
		temp["AA"] = float(output_a_b[0])
		temp["MW"] = float(output_a_b[1])

		final_results.append(temp)

		if(total_pairs%100 == 0):
			print(total_pairs)
			appendCSV(final_results, ',')
			del final_results[:]

	appendCSV(final_results, ',')
	sorted_result = pd.read_csv("./test_results.csv")
	sorted_result.sort_values(by=['distance'], inplace = True, ascending = True)
	sorted_result.to_csv("./test_results.csv", sep = ',')

if __name__ == '__main__':
	main()

