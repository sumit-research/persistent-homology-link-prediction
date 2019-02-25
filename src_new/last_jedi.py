import os
import sys
import sqlite3

def get_hop(conn, node1, node2):
	query_a = """ SELECT hop from nodes WHERE ID_a = ? AND ID_b = ? """ 

	input_ = (node1, node2)

	c = conn.cursor()
	c.execute(query_a, input_)
	rows = [i[0] for i in c.fetchall()]
	c.close()

	return rows[0]

def main():
	if(len(sys.argv) != 4):
		print("[Usage:] python3 last_jedi.py input_file database_loc column_number")
		exit()

	input_file = sys.argv[1]
	database_loc = sys.argv[2]
	column_number = int(sys.argv[3])

	f = open(input_file, "r")
	data = f.readlines()
	f.close()

	hop_distri = {}

	for line in data:
		line = line.strip().split()
		node1 = line[0]
		node2 = line[1]

		score = line[column_number]
		if(score == "inf" or int(score) > 100 or int(score) == -1 or int(score) == -2):
			continue
		# print(node1, node2)
		conn = sqlite3.connect(database_loc)
		hop = get_hop(conn, node1, node2)
		if(hop in hop_distri.keys()):
			hop_distri[hop] += 1
		else:
			hop_distri[hop] = 1


	for key in hop_distri.keys():
		print("{} {}".format(key, hop_distri[key]))

if __name__ == '__main__':
	main()
