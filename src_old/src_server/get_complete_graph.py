import os
import sys
from subprocess import Popen, PIPE

def main():
	if(len(sys.argv) != 2):
		print("[Usage:] python3 get_complete_graph.py data_file")

	file = str(sys.argv[1])
	nodes = []

	with open(file, "r") as f:
		for line in f:
			line = line.strip().split()
			if(len(line) == 1):
				node = line[0]
				node = node[:-1]
				nodes.append(int(node))
			else:
				nodes.append(int(line[0]))

	nodes = set(nodes)

	node_1 = file.strip().split('_')[-3]
	node_2 = file.strip().split('_')[-2]
	hop = file.strip().split('_')[-1]

	out_file = "/Users/admin/Desktop/Project/outputs/n_5/compGraph_"+str(node_1) + "_" + str(node_2)
	os.system("touch " + out_file)

	with open(out_file, "w") as f:
		for node_a in nodes:
			f.write(str(node_a) + ":\n")
			for node_b in nodes:
				if(node_a == node_b):
					continue
				f.write(str(node_b) + " " + str(1) + str('\n'))
	
	command1 = "g++ -std=c++14 /Users/admin/Desktop/Project/src/johnson.cpp -o ~/Desktop/Project/src/johnson"
	in_file = out_file
	out_file_full = "~/Desktop/Project/outputs/n_" + str(hop) + "/apsp_complete_full_" + str(node_1) + "_" + str(node_2)
	out_file_sparse = "~/Desktop/Project/outputs/n_" + str(hop) + "/apsp_complete_sparse_" + str(node_1) + "_" + str(node_2)
	command = "~/Desktop/Project/src/johnson " + in_file + " " + out_file_full + " " + out_file_sparse

	os.system(command1)
	os.system(command)

	# run dipha and get persistence diagrams for both 

	in_file = out_file_full
	out_file = "/Users/admin/Desktop/Project/outputs/n_" + str(hop) + "/dipha_complete_" + str(node_1) + "_" + str(node_2)
	command = "dipha --upper_dim 2 " + in_file + " " + out_file

	os.system(command)

if __name__ == '__main__':
	main()