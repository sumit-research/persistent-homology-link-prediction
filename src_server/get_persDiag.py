import sys
import os

# get input nodes and define neighbourhood
# input filename
# input node -- a, b
# n-hop neighbourhood -- n

def main():
	if(len(sys.argv) != 4 and len(sys.argv) != 5):
		print("[Usage:] python3 script.py data_file node_a n")
		print("[Usage:] python3 script.py data_file node_a node_b n")
		exit()

	data_file = sys.argv[1]
	nodes = []
	nodes.append(int(sys.argv[2]))
	hop = int(sys.argv[len(sys.argv)-1])

	if(len(sys.argv) == 5):
		nodes.append(int(sys.argv[3]))

		# getting n-hop neighbourhood of the nodes

		out_file= "/home/deepak/Project/files/outputs/n_"  + str(hop) + "/n_hop_" + str(nodes[0]) + "_" + str(nodes[1])
		command= "/home/deepak/Project/code/src_server/n_hop_multi " + data_file + " " + out_file + " " + str(nodes[0]) + " " + str(nodes[1]) + " " + str(hop) 

		os.system(command)


		# get all pair shortest path distances using johnson algorithm for n-hop subgraph

		in_file = out_file
		out_file_full = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/apsp_full_" + str(nodes[0]) + "_" + str(nodes[1])
		out_file_sparse = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/apsp_sparse_" + str(nodes[0]) + "_" + str(nodes[1])
		command = "/home/deepak/Project/code/src_server/johnson " + in_file + " " + out_file_full + " " + out_file_sparse

		os.system(command)

		# run dipha and get persistence diagram

		in_file = out_file_full
		out_file = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/dipha_" + str(nodes[0]) + "_" + str(nodes[1])
		command = "mpiexec -n 12 dipha --upper_dim 2 " + in_file + " " + out_file

		os.system(command)
	else:

		# getting n-hop neighbourhood of the nodes
		command1 = "g++ -std=c++14 /home/deepak/Project/code/src_server/n_hop_multi.cpp -o /home/deepak/Project/code/src_server/n_hop_multi"
		out_file= "/home/deepak/Project/files/outputs/n_"  + str(hop) + "/n_hop_" + str(nodes[0]) + "_" + str(hop)
		command= "/home/deepak/Project/code/src_server/n_hop_multi " + data_file + " " + out_file + " " + str(nodes[0]) + " " + str(hop) 

		os.system(command1)
		os.system(command)

		# get all pair shortest path distances using johnson algorithm for n-hop subgraph

		command2 = "g++ -std=c++14 /home/deepak/Project/code/src_server/johnson.cpp -o /home/deepak/Project/code/src_server/johnson"
		in_file = out_file
		out_file_full = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/apsp_full_" + str(nodes[0])
		out_file_sparse = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/apsp_sparse_" + str(nodes[0])
		command = "/home/deepak/Project/code/src_server/johnson " + in_file + " " + out_file_full + " " + out_file_sparse

		os.system(command2)
		os.system(command)

		# run dipha and get persistence diagram

		in_file = out_file_full
		out_file = "/home/deepak/Project/files/outputs/n_" + str(hop) + "/dipha_" + str(nodes[0])
		command = "mpiexec -n 12 dipha --upper_dim 2 " + in_file + " " + out_file

		os.system(command)

 
if __name__ == '__main__':
	main()
