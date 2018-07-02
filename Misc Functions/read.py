v = {}
with open('./coracites.txt') as file:
	for line in file:
		graph = list(map(int, line.strip().split('\t')))
		if(graph[1] in v):
			v[graph[1]].append(graph[0])
		else:
			v[graph[1]] = [graph[0]]


for k in v:
	print("%d:" % k)
	for i in v[k]:
		print("%d %d" % (i, 1))