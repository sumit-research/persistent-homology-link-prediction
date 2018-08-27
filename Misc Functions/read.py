from collections import OrderedDict
v = OrderedDict([])
with open('/home/deepak/Project/files/data/HomoSapiens/dataOriginal.txt') as file:
	for line in file:
		graph = line.strip().split('\t')
		# if((graph[0], graph[1]) in v):
		# 	continue;
		# else:
		# 	print("%s %s" % (graph[0], graph[1]))
		# 	v[graph[0], graph[1]] = 1
		if(graph[0] in v.keys()):
			v[graph[0]].append(graph[1])
		else:
			v[graph[0]] = [graph[1]]

		# if(graph[1] in v.keys()):
		# 	v[graph[1]].append(graph[0])
		# else:
		# 	v[graph[1]] = [graph[0]]

for k in v:
	print("%s:" % k)
	for i in v[k]:
		print("%s %s" % (i, 1))
