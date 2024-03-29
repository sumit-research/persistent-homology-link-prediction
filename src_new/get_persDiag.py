import sys
import os
import struct

# get input nodes and define neighbourhood
# input filename
# input node -- a, b
# n-hop neighbourhood -- n

def main():
	if(len(sys.argv) != 5 and len(sys.argv) != 6 and len(sys.argv) != 7):
		print("[Usage:] python3 script.py dataset_name data_file node_a n")
		print("[Usage:] python3 script.py dataset_name data_file node_a node_b n")
		print("Options: ")
		print("--remove -> Used when the input data is modified by removing an edge")
		exit()


	remove = False

	if(sys.argv[1] == '--remove'):
		remove = True
		sys.argv = sys.argv[2:]
	else:
		sys.argv = sys.argv[1:]

	dataset_name = sys.argv[0]
	data_file = sys.argv[1]
	nodes = []
	nodes.append(sys.argv[2])
	hop = int(sys.argv[len(sys.argv)-1])


	if(len(sys.argv) == 5):
		nodes.append(sys.argv[3])
		# getting n-hop neighbourhood of the nodes

		out_file= "/home/deepak/Project/files/outputs/"+dataset_name+"/n_"  + str(hop) + "/n_hop_" + str(nodes[0]) + "_" + str(nodes[1])
		if(remove):
			out_file= "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"  + str(hop) + "/n_hop_" + str(nodes[0]) + "_" + str(nodes[1])

		command1 = "g++ -std=c++14 /home/deepak/Project/code/src_ripser/n_hop_multi.cpp -o /home/deepak/Project/code/src_ripser/n_hop_multi"
		command= "/home/deepak/Project/code/src_ripser/n_hop_multi " + dataset_name + " " + data_file + " " + out_file + " " + str(nodes[0]) + " " + str(nodes[1]) + " " + str(hop)
		if(remove):
			command= "/home/deepak/Project/code/src_ripser/n_hop_multi --remove " + dataset_name + " " + data_file + " " + out_file + " " + str(nodes[0]) + " " + str(nodes[1]) + " " + str(hop) 
 
		os.system(command1)
		os.system(command)

		# get all pair shortest path distances using johnson algorithm for n-hop subgraph

		in_file = out_file
		out_file_full = "/home/deepak/Project/files/outputs/"+dataset_name+"/n_" + str(hop) + "/apsp_full_" + str(nodes[0]) + "_" + str(nodes[1])
		out_file_sparse = "/home/deepak/Project/files/outputs/"+dataset_name+"/n_" + str(hop) + "/apsp_sparse_" + str(nodes[0]) + "_" + str(nodes[1])
		if(remove):
			out_file_sparse = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/apsp_sparse_" + str(nodes[0]) + "_" + str(nodes[1])
			out_file_full = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/apsp_full_" + str(nodes[0]) + "_" + str(nodes[1])

		command2 = "g++ -std=c++14 /home/deepak/Project/code/src_ripser/johnson.cpp -o /home/deepak/Project/code/src_ripser/johnson"
		command = "/home/deepak/Project/code/src_ripser/johnson " + dataset_name + " " + in_file + " " + out_file_full + " " + out_file_sparse

		os.system(command2)
		os.system(command)

		# run dipha and get persistence diagram

		in_file = out_file_full
		out_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/n_" + str(hop) + "/ripser_" + str(nodes[0]) + "_" + str(nodes[1]) + ".txt"
		if(remove):
			out_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/ripser_" + str(nodes[0]) + "_" + str(nodes[1]) + ".txt"

		# f = open(in_file, "rb")
		# data = f.read()
		# num_processors = struct.unpack('<q' , data[16:24])[0]
		# if(num_processors > 20):
		# 	num_processors = 20
		# if(num_processors == 0):
		# 	num_processors+=1
		_format = "lower-distance"
		with open(in_file, "r") as apsp_file:
			first_line = apsp_file.readline()
			apsp_file.close()
			if(first_line == '0'):
				_format = "distance"

		command = "ripser --dim 1 --threshold 4 --format " + _format + " " + in_file + " > " + out_file

		os.system(command)

	else:

		# getting n-hop neighbourhood of the nodes
		command1 = "g++ -std=c++14 /home/deepak/Project/code/src_ripser/n_hop_multi.cpp -o /home/deepak/Project/code/src_ripser/n_hop_multi"
		out_file= "/home/deepak/Project/files/outputs/"+dataset_name+"/n_"  + str(hop) + "/n_hop_" + str(nodes[0]) + "_" + str(hop)
		if(remove):
			out_file= "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"  + str(hop) + "/n_hop_" + str(nodes[0]) + "_" + str(hop)

		command= "/home/deepak/Project/code/src_ripser/n_hop_multi " + dataset_name + " " + data_file + " " + out_file + " " + str(nodes[0]) + " " + str(hop) 
		if(remove):
			command= "/home/deepak/Project/code/src_ripser/n_hop_multi --remove " + dataset_name + " " + data_file + " " + out_file + " " + str(nodes[0]) + " " + str(hop) 


		os.system(command1)
		os.system(command)

		# get all pair shortest path distances using johnson algorithm for n-hop subgraph

		command2 = "g++ -std=c++14 /home/deepak/Project/code/src_ripser/johnson.cpp -o /home/deepak/Project/code/src_ripser/johnson"
		in_file = out_file
		out_file_full = "/home/deepak/Project/files/outputs/"+dataset_name+"/n_" + str(hop) + "/apsp_full_" + str(nodes[0])
		out_file_sparse = "/home/deepak/Project/files/outputs/"+dataset_name+"/n_" + str(hop) + "/apsp_sparse_" + str(nodes[0])
		if(remove):
			out_file_full = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/apsp_full_" + str(nodes[0])
			out_file_sparse = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/apsp_sparse_" + str(nodes[0])

		command = "/home/deepak/Project/code/src_ripser/johnson " + dataset_name + " " + in_file + " " + out_file_full + " " + out_file_sparse

		os.system(command2)
		os.system(command)

		# run dipha and get persistence diagram

		in_file = out_file_full
		out_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/n_" + str(hop) + "/ripser_" + str(nodes[0]) + ".txt"
		if(remove):
			out_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/ripser_" + str(nodes[0]) + ".txt"

		# f = open(in_file, "rb")
		# data = f.read()
		# num_processors = struct.unpack('<q' , data[16:24])[0]
		# if(num_processors > 20):
		# 	num_processors = 20
		# if(num_processors == 0):
		# 	num_processors+=1
		_format = "lower-distance"
		with open(in_file, "r") as apsp_file:
			first_line = apsp_file.readline()
			apsp_file.close()
			if(first_line == '0'):
				_format = "distance"

		command = "ripser --dim 1 --threshold 4 --format " + _format + " " + in_file + " > " + out_file

		os.system(command)

 
if __name__ == '__main__':
	main()
