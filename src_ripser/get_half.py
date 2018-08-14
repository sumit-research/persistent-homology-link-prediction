file_name = "/home/deepak/Project/files/outputs/cora/dumped.txt"

pairs = {}
with open(file_name, "r") as f:
	for line in f:
		line = line.strip().split()
		if((line[2], line[1]) in pairs):
			continue
		else:
			pairs[(line[1], line[2])] = line[0]

for i in pairs.keys():
	print("%s %s %s", (pairs[i], i[0], i[1]))