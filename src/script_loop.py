import sys
import os
from subprocess import Popen, PIPE
from operator import itemgetter

def main():
	if(len(sys.argv) != 4):
		print("[Usage:] python3 script.py data_file node_a n")
		exit()

	data_file = sys.argv[1]
	node_a = int(sys.argv[2])
	hop = int(sys.argv[3])

	found = set([node_a])
	wasserstein_0 = []
	wasserstein_1 = []
	bottleneck_0 = []
	bottleneck_1 = []

	os.system("mkdir ~/Desktop/Project/outputs/n_" + str(hop))
	os.system("python3 get_persDiag.py ~/Desktop/Project/data/cora/coradata.txt " + str(node_a) + " " + str(hop))

	nodes_completed = 0
	with open(data_file, "r") as f:
		for line in f:
			l = line.strip().split()
			if(len(l) == 1):
				s = int(l[0][0:-1])
				if(s not in found):
					nodes_completed += 1
					os.system("python3 get_persDiag.py ~/Desktop/Project/data/cora/coradata.txt " + str(s) + " " + str(hop))
					dgm1_file = "/Users/admin/Desktop/Project/outputs/n_"+  str(hop) + "/dipha_" + str(node_a)
					dgm2_file = "/Users/admin/Desktop/Project/outputs/n_"+  str(hop) + "/dipha_" + str(s)
					process = Popen(["python3", "compare_diagram.py", dgm1_file, dgm2_file, str(2),str(0)], stdout=PIPE)
					process1 = Popen(["python3", "compare_diagram.py", dgm1_file, dgm2_file, str(2), str(1)], stdout=PIPE)
					found.add(s)
					(output_0,err) = process.communicate()
					(output_1,err) = process1.communicate()
					get_score_0 = output_0.strip().splitlines()
					get_score_1 = output_1.strip().splitlines()

					wasserstein_0.append((float(get_score_0[0]), s))
					bottleneck_0.append((float(get_score_0[1]), s))
					wasserstein_1.append((float(get_score_1[0]), s))
					bottleneck_1.append((float(get_score_1[1]), s))
					if(nodes_completed%100 == 0):
						print(nodes_completed)

	wasserstein_0 = sorted(wasserstein_0, key = itemgetter(0))
	wasserstein_1 = sorted(wasserstein_1, key = itemgetter(0))
	bottleneck_0 = sorted(bottleneck_0, key= itemgetter(0))
	bottleneck_1 = sorted(bottleneck_1, key = itemgetter(0))

	print("dim 0 wasserstein: ")
	for i in wasserstein_0:
		print(i)
	print("\ndim 1 wasserstein: ")
	for i in wasserstein_1:
		print(i)
	print("\ndim 0 bottleneck: ")
	for i in bottleneck_0:
		print(i)
	print("\ndim 1 bottleneck: ")
	for i in bottleneck_1:
		print(i)

if __name__ == '__main__':
	main()