import sys
import os

# get input nodes and define neighbourhood
# input filename
# input node -- a, b
# n-hop neighbourhood -- n

def main():
	if(len(sys.argv) != 5):
		print("[Usage:] python3 script.py data_file node_a node_b n")
		exit()

	data_file = sys.argv[1]
	node_a = int(sys.argv[2])
	node_b = int(sys.argv[3])
	hop = int(sys.argv[4])

	os.system("python3 ~/Desktop/Project/src/get_persDiag.py " + data_file + " " + str(node_a) + " " + str(hop))
	os.system("python3 ~/Desktop/Project/src/get_persDiag.py " + data_file + " " + str(node_b) + " " + str(hop))

	dgm1_file = "~/Desktop/Project/outputs/n_" + str(hop) + "/dipha_" + str(node_a)
	dgm2_file = "~/Desktop/Project/outputs/n_" + str(hop) + "/dipha_" + str(node_b)

	os.system("python3 compare_diagram.py " + dgm1_file + " " + dgm2_file + " 2 0")
	os.system("python3 compare_diagram.py " + dgm1_file + " " + dgm2_file + " 2 1")

 
if __name__ == '__main__':
	main()
