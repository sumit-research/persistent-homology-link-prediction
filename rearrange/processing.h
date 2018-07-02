#ifndef _PROCESSING_INCLUDED_
#define _PROCESSING_INCLUDED_

#include "includes.h"

intt input(string inputFilename, vector<vector<intt> >& adj, vector<vector<intt> >& in, map<intt, intt>& to_indices, map<intt, intt>& to_node);

intt input(string inputFilename, vector<vector< ii > >& adj, vector<vector< ii > >& in, map<intt, intt>& to_indices, map<intt, intt>& to_node);

void calculateFrequency(vector<vector< intt > > vec, map<intt,intt>& frequency, int num_nodes);

void calculateFrequency(vector<vector< ii > > vec, map<intt,intt>& frequency, int num_nodes);

int write_txt(string filename, vector<vector<intt> > adj, vector<vector<intt> > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes, int frequ);

int write_txt(string filename, vector<vector< ii > > adj, vector<vector< ii > > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes, int frequ);

int write_txt(string filename, vector<vector<intt> > adj, vector<vector<intt> > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes);

int write_txt(string filename, vector<vector< ii > > adj, vector<vector< ii > > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes);

void write_sparse(string filename, vector<intt> connections, vector<intt> indices, vector<intt> weight, intt num_nodes)

#endif