import sys
import struct
from time import time
from rpy2.robjects.packages import importr
import rpy2.robjects as ro
from rpy2.robjects import numpy2ri
import numpy as np

def get_dgm(input_filename):
	f = open(input_filename, "r")
	
	# read dipha magic number, file type and number of nodes
	# args = struct.unpack('<'+3*'q', data[0:24])
	# dipha = args[0]
	# file_type = args[1]
	# num_nodes = args[2]

	# get positions of starting bits for each variable
	# dims_ind = list(range(24, (num_nodes+1)*3*8, 24))
	# birth_ind = list(range(32, (num_nodes+1)*3*8, 24))
	# death_ind = list(range(40, (num_nodes+1)*3*8, 24))
	
	dgm = []

	for line in f:
		line = list(map(float,line.strip().split(", ")))
		# line[0] = int(line[0])
		dgm.append(line)
	f.close()

	# for i,j,k in zip(dims_ind,birth_ind,death_ind):
	# 	dims = struct.unpack('<q', data[i:i+8])[0]
	# 	birth = struct.unpack('<d', data[j:j+8])[0]
	# 	death = struct.unpack('<d', data[k:k+8])[0]

	# 	if(dims < 0):
	# 		dims = -dims - 1

	# 	temp = [dims, birth, death]
	# 	dgm.append(temp)
	return np.array(dgm)

def get_bottleneck_distance(dim, dgm1, dgm2):
	TDA = importr('TDA')

	# print diagrams -- dimension, birth value, death value
	# print(ro.r["dgm1"])
	# print(ro.r["dgm2"])

	# get bottleneck distance
	f = TDA.bottleneck(dgm1, dgm2, dimension = dim)
	return f[0]

def get_wasserstein_distance(p, dim, dgm1, dgm2):
	TDA = importr('TDA')	
	
	# print diagrams -- dimension, birth value, death value
	# print(ro.r["dgm1"])
	# print(ro.r["dgm2"])

	# get wasserstein distance
	f = TDA.wasserstein(dgm1,dgm2, p = p, dimension = dim)
	return f[0]

def main():
	if(len(sys.argv) != 5):
		print("[Usage:] python3 compare_diagram.py dgm1_file dgm2_file p dimension")
		exit()

	# start = time()
	# get params
	dgm1_file = sys.argv[1]
	dgm2_file = sys.argv[2]
	p = float(sys.argv[3])
	dim = float(sys.argv[4])

	# get dimension, birth, death of all the points
	dgm1 = get_dgm(dgm1_file)
	dgm2 = get_dgm(dgm2_file)
	num_points_diag = len(dgm1)
	
	# for conversion of numpy matrix to r array
	ro.conversion.py2ri = numpy2ri
	ro.numpy2ri.activate()

	# convert numpy array to r matrix
	m1 = ro.r.matrix(dgm1, nrow=dgm1.shape[0], ncol=dgm1.shape[1])
	m2 = ro.r.matrix(dgm2, nrow=dgm2.shape[0], ncol=dgm2.shape[1])

	# assign variable in R workspace
	ro.r.assign("m1", m1)
	ro.r.assign("m2", m2)

	# print wasserstein distance
	# start = time()
	# print("%f" % (get_wasserstein_distance(p, dim, ro.r["m1"], ro.r["m2"])))
	print("%f" % (0.0))
	# end = time()
	# print("wasserstein-> ", t+end-start)
	# start = time()
	print("%f" % (get_bottleneck_distance(dim, ro.r["m1"], ro.r["m2"])))
	# end = time()
	# print("bottleneck-> ", t+end-start)

if __name__ == '__main__':
	main()
