import sys
import os
import struct
import pandas as pd
import numpy as np
import random
from collections import OrderedDict
from subprocess import Popen, PIPE
# get input nodes and define neighbourhood
# input filename
# n-hop neighbourhood -- n

def appendCSV(final_results, sep, out_file):
	result = pd.DataFrame(final_results, columns = final_results[0].keys())
	if(os.path.isfile(out_file)):
		result.to_csv(out_file, mode = 'a', header = False, index = False)
	else:
		result.to_csv(out_file, sep = sep, index=False)

def get_nodes(data, node):
	nodes = set([])
	nodes = [l[2] for l in data if(l[1] == node and int(l[0]) <= 5)]
	return nodes

def get_position(results, node, metric):
	ascending = True
	if(metric == "AA" or metric == "MW"):
		ascending = False
	sorted_results = results.sort_values(by=[metric], ascending = ascending)
	list_values = list(sorted_results["node_b"])
	return list_values.index(node)

def get_random_edges(data, sample):
	source, dest = "",""
	n_data = []

	for line in data:
		l = line.strip().split()
		if(l[0][-1] == ':'):
			source = l[0][0:-1]
			
		else:
			dest = l[0]
			n_data.append([source, dest])

	return random.sample(n_data, sample)

def remove_edge(data, new_data, node_a, node_b):
	w = open(new_data, "w")
	flag = False
	source, dest = "",""
	n_data = []

	for line in data:
		l = line.strip().split()
		if(l[0][-1] == ':'):
			n_data.append(line)
			source = l[0][0:-1]
			
		else:
			dest = l[0]
			if(source == node_a and dest == node_b):
				continue
			else:
				n_data.append(line)

	n_data.append(str(node_b)+":")

	w.writelines(n_data)
	w.close()

def main():
	if(len(sys.argv) != 6):
		print("[Usage:] python3 script.py data_file edges_out output_file n sample_size")
		exit()

	data_file = sys.argv[1]
	edges_out = sys.argv[2]
	output_file = sys.argv[3]
	hop = int(sys.argv[4])
	sample_size = int(sys.argv[5])

	if(os.path.exists("/home/deepak/Project/files/outputs/removed_edge_" + str(hop)) == False):
		os.system("mkdir /home/deepak/Project/files/outputs/removed_edge_" + str(hop))

	# compile all c++ files


	# get all the reachable pairs in the graph to test

	dumped_file = "/home/deepak/Project/files/outputs/dumped_copy.txt"
	# os.system("/home/deepak/Project/code/src_server/johnson --dump_pairs " + data_file + " /home/deepak/Project/files/outputs/removed_edge_" + str(hop) + "/global.txt /home/deepak/Project/files/outputs/removed_edge_" + str(hop) + "/global_sparse.txt" )
	df = open(dumped_file, "r")
	dumped_data = df.readlines()
	dumped_data = [line.strip().split() for line in dumped_data]
	df.close()


	# data = "/home/deepak/Project/code/src_server/random_select.txt"


	# read data from input file 
	f = open(data_file, "r")
	data = f.readlines()
	f.close()

	random_edges = []
	num_edges = 0
	resume_pos = 0

	if(os.path.isfile(edges_out)):
		t = open(edges_out, "r")
		random_edges = t.readlines()
		random_edges = [e.strip().split() for e in random_edges]
		t.close()

	else:
	# get 100 edges at random
		random_edges = get_random_edges(data, sample_size)
		t = open(edges_out, "w")
		for line in random_edges:
			t.write(str(line[0] + " " + str(line[1] + '\n')))
		t.close()

	if(os.path.isfile(output_file)):
		df = pd.read_csv(output_file)
		resume_pos = df.shape[0]

	final_results = []

	metrics = ["W_a_0", "W_a_1", "W_b_0", "W_b_1", "B_a_0", "B_a_1", "B_b_0", "B_b_1", "W_comp_0", "W_comp_1", "B_comp_0", "B_comp_1", "AA", "MW"]

	for edge in random_edges:
		num_edges += 1

		if(num_edges <= resume_pos):
			continue

		ranking = []
		node_a = edge[0]
		node = edge[1]

		# get all the nodes at distance <= 5 from node_a
		nodes = get_nodes(dumped_data, edge[0])
		print(node_a, node)

		# remove the edge between node_a and node
		removed_edge_data = "/home/deepak/Project/files/data/modified_data.txt"
		remove_edge(data, removed_edge_data, node_a, node)

		for node_b in nodes:
			if(node_b == str(node_a)):
				continue

			temp = OrderedDict()
			temp["node_a"] = node_a
			temp["node_b"] = node_b

			# if(total_pairs <= 34800):
			# 	continue

			# define file names for persistence diagrams

			dgm1_file = "/home/deepak/Project/files/outputs/removed_edge_" + str(hop) + "/dipha_" + str(node_a)
			dgm2_file = "/home/deepak/Project/files/outputs/removed_edge_" + str(hop) + "/dipha_" + str(node_b)
			dgmCombine_file = "/home/deepak/Project/files/outputs/removed_edge_" + str(hop) + "/dipha_" + str(node_a) + "_" + str(node_b)
			dgmComplete_file = "/home/deepak/Project/files/outputs/removed_edge_" + str(hop) + "/dipha_complete_" + str(node_a) + "_" + str(node_b)

			# obtain persistence diagrams for node_a, node_b and combined

			os.system("python3 /home/deepak/Project/code/src_server/get_persDiag.py --remove " + removed_edge_data + " " + str(node_a) + " " + str(hop))
			os.system("python3 /home/deepak/Project/code/src_server/get_persDiag.py --remove " + removed_edge_data + " " + str(node_b) + " " + str(hop))

			os.system("python3 /home/deepak/Project/code/src_server/get_persDiag.py --remove " + removed_edge_data + " " + str(node_a) + " " + str(node_b) + " " + str(hop))

			# get persistence diagram for complete graph
			in_file = "/home/deepak/Project/files/outputs/removed_edge_" + str(hop) + "/apsp_complete_full_" + str(node_a) + "_" + str(node_b)
			f = open(in_file, "rb")
			data_p = f.read()
			num_processors = struct.unpack('<q' , data_p[16:24])[0]
			if(num_processors > 3):
				num_processors = 3
			if(num_processors == 0):
				num_processors+=1
			command = "mpiexec -n " + str(num_processors) + " dipha --upper_dim 2 " + in_file + " " + dgmComplete_file
			os.system(command)			

			# compare pairwise diagrams
			process_a_b = Popen(["/home/deepak/Project/code/src_server/baseline", removed_edge_data, "/home/deepak/Project/code/src_server/test.txt", str(node_a), str(node_b)], stdout=PIPE)
			process_a_0 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgm1_file, str(2),str(0)], stdout=PIPE)
			process_a_1 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgm1_file, str(2),str(1)], stdout=PIPE)
			process_b_0 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgm2_file, str(2),str(0)], stdout=PIPE)
			process_b_1 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgm2_file, str(2),str(1)], stdout=PIPE)
			process_complete_0 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgmComplete_file, str(2),str(0)], stdout=PIPE)
			process_complete_1 = Popen(["python3", "/home/deepak/Project/code/src_server/compare_diagram.py", dgmCombine_file, dgmComplete_file, str(2),str(1)], stdout=PIPE)
			
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

			ranking.append(temp)

		ranking = pd.DataFrame(ranking)
		results_temp = OrderedDict()
		results_temp["node_a"] = node_a
		results_temp["node_b"] = node
		for each in metrics:
			index = get_position(ranking, node, each)+1
			results_temp[each] = index

		appendCSV(results_temp, ',', output_file)



if __name__ == '__main__':
	main()
