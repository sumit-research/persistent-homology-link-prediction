import sys
import os
import csv
import pandas as pd
from collections import OrderedDict

def main():
	train_dist = OrderedDict([])
	dataset = sys.argv[1]
	train_output_file = sys.argv[2]

	f = open(dataset, 'r')

	i = 0
	for line in f:
		line = line.strip().split()

		i+=1
		if(i%1000000==0):
			print(i)

		if(int(line[0]) in train_dist):
			train_dist[int(line[0])] += 1
		else:
			train_dist[int(line[0])] = int(1)
	f.close()

	with open(train_output_file, 'w') as csv_file:
	    writer = csv.writer(csv_file)
	    for key in sorted(train_dist.keys()):
	       writer.writerow([key, train_dist[key]])


if __name__ == '__main__':
	main()