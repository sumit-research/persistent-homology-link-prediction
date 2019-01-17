import sys
import os
import struct
import random
import pandas as pd
import numpy as np
from collections import OrderedDict
from subprocess import Popen, PIPE

def find_edges(data_file):
	file = open(data_file, 'r')
	data = file.readlines()
	file.close()

	# list of all edges
	edges = []
	nodes = []

	source, dest = [0,0]

	for line in data:
		line = line.strip().split()

		if(len(line) == 1):
			# found source
			source = line[0][:-1]
			nodes.append(source)
		else:
			# found destination
			dest = line[0]
			nodes.append(dest)
			edges.append([source, dest])

	nodes = set(nodes)

	return nodes, edges



def main():
	if(len(sys.argv) != 5):
		print("[Usage:] python3 make_sample.py dataset_name sample_out_file num_positive_samples num_negative_samples\n")
		sys.exit()

	dataset_name = sys.argv[1]
	data_file = "/home/deepak/Project/files/data/"+dataset_name+"/data.txt"
	sample_out_file = sys.argv[2]
	num_positive_samples = int(sys.argv[3])
	num_negative_samples = int(sys.argv[4])

	# get all the edges
	nodes, edges = find_edges(data_file)
	nodes = list(nodes)

	print("done")

	positive_edges = list(random.sample(edges, num_positive_samples))
	negative_edges = []

	while(num_negative_samples):
		a = np.random.randint(low = 1, high = len(nodes)+1)
		b = np.random.randint(low = 1, high = len(nodes)+1)

		if(nodes[a] == nodes[b] or ([nodes[a], nodes[b]] in edges)):
			continue;
		else:
			negative_edges.append([nodes[a], nodes[b]])
			num_negative_samples-=1
			
	labels = np.ones(num_positive_samples, dtype = int)
	labels = np.append(labels,np.zeros(len(negative_edges),dtype = int))

	positive_edges.extend(negative_edges)
	sample_edges = positive_edges

	f = open(sample_out_file, 'w')
	for i in range(len(sample_edges)):
		print(sample_edges[i])
		f.write(str(labels[i]) + " " + str(sample_edges[i][0]) + " " + str(sample_edges[i][1]) + '\n')
	f.close()

if __name__ == '__main__':
	main()
