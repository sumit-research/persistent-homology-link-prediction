

# Precise method, which guarantees v = v1 when t = 1.
def lerp( v0, v1, t):
  # print "LERP"
  # print v0
  # print v1
  # print t
  # print float( ( 1.0 - t) * v0) + float(t * v1)
  return (1 - t) * v0 + t * v1


def get_LERP(x):
	# Largest pd_distance
	C = 50
	# Take C a "very big number" (Largest PD distance), set f(C) = 0, f(1/C) = 1, 
	# then f(x) = 0 for x >= C, f(x) = 1 for x <= 1/C, and interpolate linearly between 1/C and C.

	# PD distance to score...
	x = 4.25

	score = -1

	if x >= C:
		score = 0.0
		norm_score = 0.0
	if x <= (1/C):
		score = 1.0
		norm_score = 1.0

	if score == -1:
		score = lerp( 1/float(C), C, float(x) / float( C - 1/ float(C))  )
		norm_score = 1 - float(score) / float( C - 1/ float(C))

	return norm_score