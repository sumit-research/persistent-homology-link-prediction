from collections import OrderedDict
v = OrderedDict([])
with open('/home/deepak/Project/files/data/fb15K-237/train.txt') as file:
	for line in file:
		graph = line.strip().split('\t')
		# print("%s %s %s" % (graph[0], graph[2]))
		if(graph[0] in v):
			v[graph[0]].append(graph[2])
		else:
			v[graph[0]] = [graph[2]]


for k in v:
	print("%s:" % k)
	for i in v[k]:
		print("%s %s" % (i, 1))
