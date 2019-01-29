import sys
import os
import struct
import pandas as pd
import numpy as np
import random
import sqlite3
from math import sqrt
from time import time
from collections import OrderedDict
from subprocess import Popen, PIPE
# get input nodes and define neighborhood
# input filename
# n-hop neighborhood -- n

def create_table(df, conn):
	
	sql_table = """ CREATE TABLE IF NOT EXISTS nodes(
					distance float,
					hop int,
					ID_a text,
					ID_b text); """
	c = conn.cursor()
	c.execute(sql_table)
	df.to_sql("nodes", conn, if_exists='replace', index = False)

	create_index = """ CREATE INDEX IF NOT EXISTS index_nodes ON nodes(ID_a, ID_b, distance, hop) """

	c.execute(create_index)
	c.close()
	conn.commit()
	return conn

def appendCSV(final_results, sep, out_file):
	result = pd.DataFrame(final_results, columns = final_results.keys(), index = [0])
	if(os.path.isfile(out_file)):
		result.to_csv(out_file, mode = 'a', header = False, index = False)
	else:
		result.to_csv(out_file, sep = sep, index=False)



def get_nodes(conn, node, hop, var):
	# conn = sqlite3.connect(database)

	query_a = """ SELECT ID_b from nodes WHERE ID_a = ? AND ID_b != Id_a AND hop <= ? """ 

	input_ = (node, hop)

	c = conn.cursor()
	c.execute(query_a, input_)
	rows = [i[0] for i in c.fetchall()]
	c.close()

	nodes = set([])
	nodes.update(rows)
	# conn.close()

	return list(nodes)

def get_APSP(conn, nodes, hop, dataset_name):
	filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/apsp_full_" + str(nodes[0]) + ".txt"
	if(len(nodes) == 2):
		filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/apsp_full_" + str(nodes[0]) + "_" + str(nodes[1]) + ".txt"
	
	f = open(filename,"w")
	_format = "lower-distance"

	query = """ SELECT n_hood1.A AS F, n_hood2.B, nodes.hop, nodes.distance 
				FROM
				(SELECT ID_b AS A FROM nodes WHERE (ID_a = ?) AND hop <= ?) n_hood1,
				(SELECT ID_b AS B FROM nodes WHERE (ID_a = ?) AND hop <= ?) n_hood2,
				nodes
				WHERE n_hood1.A > n_hood2.B AND nodes.ID_a = n_hood1.A AND nodes.ID_b = n_hood2.B ORDER BY F ASC; """
	_input = (nodes[0], hop, nodes[0], hop,)

	if(len(nodes) == 2):
		query = """ SELECT n_hood1.A AS F, n_hood2.B, nodes.hop, nodes.distance 
				FROM
				(SELECT DISTINCT(ID_b) AS A FROM nodes WHERE (ID_a = ? OR ID_a = ?) AND hop <= ?) n_hood1,
				(SELECT DISTINCT(ID_b) AS B FROM nodes WHERE (ID_a = ? OR ID_a = ?) AND hop <= ?) n_hood2,
				nodes
				WHERE n_hood1.A >= n_hood2.B AND nodes.ID_a = n_hood1.A AND nodes.ID_b = n_hood2.B ORDER BY F ASC; """
		_input = (nodes[0], nodes[1], hop, nodes[0], nodes[1], hop,)



	curr = conn.cursor()

	curr.execute(query, _input)
	sp = curr.fetchall()
	i,j,s = [0,1,0]

	if(len(nodes)==2):
		complete_filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/apsp_complete_full_" + str(nodes[0]) + "_" + str(nodes[1]) + ".txt"
		d = open(complete_filename, "w")
		num = (sqrt(1+8*len(sp)) - 1)/2
		distances = ""
		for i in range(1,int(num)):
			dis = "1, "
			dis = dis*(i-1)
			dis += "1\n"
			distances += dis
		d.write(distances)
		d.close()

	distances = ""
	if(len(sp) == 0):
		_format = "distance"
		f.write("0.0\n")
		f.close()
		curr.close()
		return _format
	else:
		for i,x in enumerate(sp):
			s += 1
			if(s == j*(j+1)/2):
				if(x[3] == 0.0):
					distances += "\n"
				else:
					distances += str(x[3]) + "\n"
				j+=1
			else:
				if(x[3] != 0.0):
					distances += str(x[3]) + ", "
		f.write(distances)



	curr.close()
	# conn.close
	f.close()

	return _format

def get_persDiag(conn, nodes, hop, dataset_name):
	filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/ripser_" + str(nodes[0]) + ".txt"
	if(len(nodes) == 2):
		filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/ripser_" + str(nodes[0]) + "_" + str(nodes[1]) + ".txt"

	in_filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/apsp_full_" + str(nodes[0]) + ".txt"
	if(len(nodes) == 2):
		in_filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/apsp_full_" + str(nodes[0]) + "_" + str(nodes[1]) + ".txt"

	_format = get_APSP(conn, nodes, hop, dataset_name)

	if(_format == "distance"):
		f = open(filename,"w")
		f.write("0, 0, 4\n")
		f.close()
	else:
		command = "ripser --dim 1 --threshold 4 --format " + _format + " " + in_filename + " > " + filename
		os.system(command)
	
	if(len(nodes) == 2):
		complete_in_filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/apsp_complete_full_" + str(nodes[0]) + "_" + str(nodes[1]) + ".txt"
		complete_out_filename = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_"+ str(hop) + "/ripser_complete_" + str(nodes[0]) + "_" + str(nodes[1]) + ".txt"
		command = "ripser --dim 1 --threshold 4 --format lower-distance " + complete_in_filename + " > " + complete_out_filename
		os.system(command)



def get_position(results, node, metric):
	ascending = True
	if(metric == "AA" or metric == "MW"):
		ascending = False
	sorted_results = results.sort_values(by=[metric], ascending = ascending)
	list_values = list(sorted_results["node_b"])
	return list_values.index(node)

def get_random_edges(data, sample):
	source, dest = "",""
	n_data = []

	for line in data:
		l = line.strip().split()
		if(l[0][-1] == ':'):
			source = l[0][0:-1]
			
		else:
			dest = l[0]
			n_data.append([source, dest])

	return random.sample(n_data, sample)

def remove_edge(data, new_data, node_a, node_b):
	w = open(new_data, "w")
	flag = False
	source, dest = "",""
	n_data = []

	for line in data:
		l = line.strip().split()
		if(l[0][-1] == ':'):
			n_data.append(line)
			source = l[0][0:-1]
			
		else:
			dest = l[0]
			if(source == node_a and dest == node_b):
				continue
			else:
				n_data.append(line)

	n_data.append(str(node_b)+":")

	w.writelines(n_data)
	w.close()

def main():
	if(len(sys.argv) != 7):
		print("[Usage:] python3 script_2.py dataset_name edges_out output_file n sample_size make_table")
		exit()

	total_time = 0.0
	start = time()

	dataset_name = sys.argv[1]
	edges_out = sys.argv[2]
	output_file = sys.argv[3]
	hop = int(sys.argv[4])
	sample_size = int(sys.argv[5])
	make_table = sys.argv[6]
	if(make_table == "1"):
		make_table = True
	else:
		make_table = False

	data_file = "/home/deepak/Project/files/data/"+dataset_name+"/data.txt"


	# check if the folder exists
	if(os.path.exists("/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop)) == False):
		os.system("mkdir /home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop))

	# compile all c++ files


	# get all the reachable pairs in the graph to test
	dumped_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/dumped.txt"
	# os.system("/home/deepak/Project/code/src_ripser/johnson --dump_pairs " + data_file + " /home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/global.txt /home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/global_sparse.txt" )
	database = "/home/deepak/Project/files/outputs/"+dataset_name+"/database.db"
	
	conn = sqlite3.connect(database)
	if(make_table):
		df = pd.read_csv(dumped_file, sep=" ", names = ["distance", "hop", "ID_a", "ID_b"], dtype={"distance":float, "hop": int, "ID_a": str, "ID_b": str})
		conn = create_table(df, conn)	

	# data = "/home/deepak/Project/code/src_ripser/random_select.txt"


	# read data from input file 
	f = open(data_file, "r")
	data = f.readlines()
	f.close()

	random_edges = []
	num_edges = 0
	resume_pos = 0

	# if given edges_out already exists then it will not create a sample of random edges
	if(os.path.isfile(edges_out)):
		t = open(edges_out, "r")
		random_edges = t.readlines()
		random_edges = [e.strip().split() for e in random_edges]
		t.close()

	else:
	# get 100 edges at random
		random_edges = get_random_edges(data, sample_size)
		t = open(edges_out, "w")
		for line in random_edges:
			t.write(str(line[0] + " " + str(line[1] + '\n')))
		t.close()

	if(os.path.isfile(output_file)):
		df = pd.read_csv(output_file)
		resume_pos = df.shape[0]

	final_results = []

	metrics = ["W_a_0", "W_a_1", "W_b_0", "W_b_1", "B_a_0", "B_a_1", "B_b_0", "B_b_1", "W_comp_0", "W_comp_1", "B_comp_0", "B_comp_1", "AA", "MW"]

	end = time()
	print("Preprocessing time: %f" % float(end-start))

	nodes_compared = 0

	time_taken = [0.0, 0.0 , 0.0]

	for edge in random_edges:
		time_taken = [0.0, 0.0, 0.0]
		start = time()

		num_edges += 1

		if(num_edges <= resume_pos):
			continue

		ranking = []
		node_a = edge[0]
		node = edge[1]

		# get all the nodes at hop <= 5 from node_a
		nodes = get_nodes(conn, node_a, 1, True)
		nodes.append(node)
		nodes_compared = len(nodes)
		print(node_a, node)

		# remove the edge between node_a and node
		# removed_edge_data = "/home/deepak/Project/files/data/"+dataset_name+"/modified_data.txt"
		# remove_edge(data, removed_edge_data, node_a, node)

		# os.system("python3 /home/deepak/Project/code/src_ripser/get_persDiag.py --remove " + dataset_name + " " + removed_edge_data + " " + str(node_a) + " " + str(hop))
		startin = time()
		dgm1_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/ripser_" + str(node_a) + ".txt"

		if(os.path.exists(dgm1_file) == False):
			get_persDiag(conn, [node_a], hop, dataset_name)
		endin = time()
		time_taken[0] += (endin-startin)

		for node_b in nodes:
			if(node_b == str(node_a)):
				continue

			temp = OrderedDict()
			temp["node_a"] = node_a
			temp["node_b"] = node_b
			# print(node_a, node_b)

			# if(total_pairs <= 34800):
			# 	continue

			# define file names for persistence diagrams

			dgm2_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/ripser_" + str(node_b) + ".txt"
			dgmCombine_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/ripser_" + str(node_a) + "_" + str(node_b) + ".txt"
			dgmComplete_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/ripser_complete_" + str(node_a) + "_" + str(node_b) + ".txt"

			# obtain persistence diagrams for node_a, node_b and combined

			# os.system("python3 /home/deepak/Project/code/src_ripser/get_persDiag.py --remove " + dataset_name + " " + removed_edge_data + " " + str(node_b) + " " + str(hop))
			startin = time()
			if(os.path.exists(dgm2_file) == False):
				get_persDiag(conn, [node_b], hop, dataset_name)
			endin = time()
			time_taken[0] += (endin-startin)

			# os.system("python3 /home/deepak/Project/code/src_ripser/get_persDiag.py --remove " + dataset_name + " " + removed_edge_data + " " + str(node_a) + " " + str(node_b) + " " + str(hop))
			startin = time()
			if(os.path.exists(dgmCombine_file) == False):
				get_persDiag(conn, [node_a, node_b], hop, dataset_name)
			endin = time()
			time_taken[0] += (endin-startin)
			# get persistence diagram for complete graph
			# in_file = "/home/deepak/Project/files/outputs/"+dataset_name+"/removed_edge_" + str(hop) + "/apsp_complete_full_" + str(node_a) + "_" + str(node_b)
			# f = open(in_file, "rb")
			# data_p = f.read()
			# num_processors = struct.unpack('<q' , data_p[16:24])[0]
			# if(num_processors > 3):
			# 	num_processors = 3
			# if(num_processors == 0):
			# 	num_processors+=1
			# _format = "lower-distance"
			# with open(in_file, "r") as apsp_file:
			# 	first_line = apsp_file.readline()
			# 	apsp_file.close()
			# 	if(first_line == '0'):
			# 		_format = "distance"

			# command = "ripser --dim 1 --threshold 4 --format " + _format + " " + in_file + " > " + dgmComplete_file
			# os.system(command)			

			# compare pairwise diagrams
			process_a_b = Popen(["/home/deepak/Project/code/src_ripser/baseline", data_file, "/home/deepak/Project/code/src_ripser/test.txt", str(node_a), str(node_b)], stdout=PIPE)
			process_a_0 = Popen(["python3", "/home/deepak/Project/code/src_ripser/compare_diagram.py", dgmCombine_file, dgm1_file, str(2),str(0)], stdout=PIPE)
			process_a_1 = Popen(["python3", "/home/deepak/Project/code/src_ripser/compare_diagram.py", dgmCombine_file, dgm1_file, str(2),str(1)], stdout=PIPE)
			process_b_0 = Popen(["python3", "/home/deepak/Project/code/src_ripser/compare_diagram.py", dgmCombine_file, dgm2_file, str(2),str(0)], stdout=PIPE)
			process_b_1 = Popen(["python3", "/home/deepak/Project/code/src_ripser/compare_diagram.py", dgmCombine_file, dgm2_file, str(2),str(1)], stdout=PIPE)
			process_complete_0 = Popen(["python3", "/home/deepak/Project/code/src_ripser/compare_diagram.py", dgmCombine_file, dgmComplete_file, str(2),str(0)], stdout=PIPE)
			process_complete_1 = Popen(["python3", "/home/deepak/Project/code/src_ripser/compare_diagram.py", dgmCombine_file, dgmComplete_file, str(2),str(1)], stdout=PIPE)
			
			startin = time()
			(output_a_b,err) = process_a_b.communicate()
			output_a_b = output_a_b.strip().splitlines()
			endin = time()
			time_taken[1] += (endin-startin)

			startin = time()
			(output_a_0,err) = process_a_0.communicate()
			output_a_0 = output_a_0.strip().splitlines()

			(output_a_1,err) = process_a_1.communicate()
			output_a_1 = output_a_1.strip().splitlines()

			(output_b_0,err) = process_b_0.communicate()
			output_b_0 = output_b_0.strip().splitlines()

			(output_b_1,err) = process_b_1.communicate()
			output_b_1 = output_b_1.strip().splitlines()

			(output_complete_0, err) = process_complete_0.communicate()
			output_complete_0 = output_complete_0.strip().split()

			(output_complete_1, err) = process_complete_1.communicate()
			output_complete_1 = output_complete_1.strip().split()
			endin = time()
			time_taken[2] += (endin-startin)

			# create a temporary dictionary
				
			temp["W_a_0"] = float(output_a_0[0])
			temp["B_a_0"] = float(output_a_0[1])

			temp["W_b_0"] = float(output_b_0[0])
			temp["B_b_0"] = float(output_b_0[1])

			temp["W_a_1"] = float(output_a_1[0])
			temp["B_a_1"] = float(output_a_1[1])

			temp["W_b_1"] = float(output_b_1[0])
			temp["B_b_1"] = float(output_b_1[1])

			temp["W_comp_0"] = float(output_complete_0[0])
			temp["B_comp_0"] = float(output_complete_0[1])

			temp["W_comp_1"] = float(output_complete_1[0])
			temp["B_comp_1"] = float(output_complete_1[1])
			
			temp["AA"] = float(output_a_b[0])
			temp["MW"] = float(output_a_b[1])

			ranking.append(temp)

		print("Nodes compared: %d" % nodes_compared)

		startin = time()
		ranking = pd.DataFrame(ranking)
		results_temp = OrderedDict()
		results_temp["node_a"] = node_a
		results_temp["node_b"] = node
		for each in metrics:
			index = get_position(ranking, node, each)+1
			results_temp[each] = index

		appendCSV(results_temp, ',', output_file)
		endin = time()
		print("PersDiag, Baselines, Comparison-> ", time_taken)
		print("Getting scores-> ", endin-startin)
		end = time()
		print("Total time-> ", end-start)
		total_time += end-start
	print(total_time)



if __name__ == '__main__':
	main()
