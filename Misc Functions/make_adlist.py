import os
import sys

def main():
	filename = sys.argv[1]
	edges = []
	nodes = []
	source = 0
	with open(filename, 'r') as f:
		for line in f:
			l = line.strip().split()
			if(len(l) == 1):
				nodes.append(l[0][:-1])
				source = l[0][:-1]
			else:
				nodes.append(l[0])
				edges.append((source, l[0]))

	nodes = set(nodes)
	for node in nodes:
		print(node)
	for edge in edges:
		print(edge[0], edge[1]);


if __name__ == '__main__':
	main()