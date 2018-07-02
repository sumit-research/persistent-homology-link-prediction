#ifndef _BASELINE_INCLUDED_
#define _BASELINE_INCLUDED_

#include "includes.h"

double adamic_adar(set<int> source_out, int dest, int source);

double milne_witten(set<int> source_in, int dest, int source, int num_nodes);

set<intt> getNodeSet(intt source, string type, string graphType);

vector< di > rankAA(intt source, string graphType);

#endif