from collections import OrderedDict
v = OrderedDict([])
with open('/home/deepak/Project/files/data/fb15k/test.txt') as file:
	for line in file:
		graph = line.strip().split('\t')
		if((graph[0], graph[2]) in v):
			continue;
		else:
			print("%s %s %s" % (1, graph[0], graph[2]))
			v[graph[0], graph[2]] = 1
		# if(graph[0] in v):
		# 	v[graph[0]].add(graph[2])
		# else:
		# 	v[graph[0]] = set([graph[2]])


# for k in v:
# 	print("%s:" % k)
# 	for i in v[k]:
# 		print("%s %s" % (i, 1))
