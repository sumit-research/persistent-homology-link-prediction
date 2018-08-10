import sys
import os
import csv
import pandas as pd
from collections import OrderedDict

def main():
	train_dist = OrderedDict([])
	test_dist = OrderedDict([])
	dataset = sys.argv[1]
	testset = sys.argv[2]
	train_output_file = sys.argv[3]
	test_output_file = sys.argv[4]

	d = open(testset, 'r')
	testdata = d.readlines()
	testdata = [e.strip().split()[1:] for e in testdata]
	d.close()

	testdict2 = dict([])
	for e in testdata:
		testdict2[e[0], e[1]] = 1


	f = open(dataset, 'r')

	i = 0
	for line in f:
		line = line.strip().split()

		i+=1
		if(i%1000000==0):
			print(i)
		if((line[1], line[2]) in testdict2.keys()):
			if(int(line[0]) in test_dist):
				test_dist[int(line[0])] += 1
			else:
				test_dist[int(line[0])] = int(1)

		if(int(line[0]) in train_dist):
			train_dist[int(line[0])] += 1
		else:
			train_dist[int(line[0])] = int(1)
	f.close()

	# result = pd.DataFrame(train_dist, columns = train_dist.keys(), index = [0])
	with open(train_output_file, 'w') as csv_file:
	    writer = csv.writer(csv_file)
	    for key in sorted(train_dist.keys()):
	       writer.writerow([key, train_dist[key]])

	with open(test_output_file, 'w') as csv_file:
	    writer = csv.writer(csv_file)
	    for key in sorted(test_dist.keys()):
	       writer.writerow([key, test_dist[key]])

	# sorted_result = pd.read_csv(train_output_file)
	# sorted_result.sort_values(by=sorted_result.columns[0], inplace = True, ascending = True)
	# sorted_result.to_csv(train_output_file, sep = ',')


if __name__ == '__main__':
	main()