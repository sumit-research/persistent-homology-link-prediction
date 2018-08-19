from collections import OrderedDict
v = OrderedDict([])
with open('/home/deepak/Project/files/data/cora/cora_train') as file:
	for line in file:
		graph = line.strip().split(' ')
		# if((graph[0], graph[1]) in v):
		# 	continue;
		# else:
		# 	print("%s %s" % (graph[0], graph[1]))
		# 	v[graph[0], graph[1]] = 1
		if(graph[0] in v):
			v[graph[0]].add(graph[1])
		else:
			v[graph[0]] = set([graph[1]])

		# if(graph[1] in v):
		# 	v[graph[1]].add(graph[0])
		# else:
		# 	v[graph[1]] = set([graph[0]])


for k in v:
	print("%s:" % k)
	for i in v[k]:
		print("%s %s" % (i, 1))
