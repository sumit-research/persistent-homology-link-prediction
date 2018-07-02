#include "includes.h"

double adamic_adar(set<intt> source_out, intt dest, intt source){

	// get N(source) intersect N(dest)
	vector<intt> common = getCommon(source_out, dest, 0);

	double aa = 0.0;

	for(vector<intt>::iterator it = common.begin(); it != common.end(); it++){
		intt node = *it;

		double indeg = getNbrCount(node, (string)"directed_in");

		if(indeg == 0.0 or indeg == 1.0)
			continue;
		else{
			aa += (1.0/log((indeg)));
		}
	}

	return aa;
}

double milne_witten(set<intt> source_in, intt dest, intt source, intt num_nodes){

	// get N'(source) intersect N'(dest)
	vector<intt> common = getCommon(source_in, dest, 1);

	if(common.size() == 0)
		return -1.0 * INT_MAX;

	double mw = 0.0;

	double Uindeg = getNbrCount(source, "directed_in");
	double Vindeg = getNbrCount(dest, "directed_in");

	cout << to_node[dest] << ' ' << Vindeg << '\n';

	double nmw = (log((double)max(Uindeg, Vindeg))) - log((double)common.size());
	double dmw = (log((double)num_nodes) - log((double)min(Uindeg, Vindeg)));

	mw = nmw/dmw;

	return mw;
}	

set<intt> getNodeSet(intt source, string type, string graphType){
	set<intt> sourceSet;

	if(type == "in"){
		for(int i = 0; i < in[source].size(); i++){
			if(graphType == "weighted")
				sourceSet.insert(in[source][i].second);
			else
				sourceSet.insert(in[source][i]);
		}
	}
	else{
		for(int i = 0; i < adj[source].size(); i++){
			if(graphType == "weighted")
				sourceSet.insert(adj[source][i].second);
			else
				sourceSet.insert(adj[source][i]);
		}
	}
}


vector< di > rankAA(intt source, string graphType){
	vector< di > aascore; // (rating, node)
	set<intt> source_out = getNodeSet(source, "out", graphType);
	
	// for every destination vertex calculate both scores and push them in an array
	for(int i = 1; i <= num_nodes; i++){
		if(i == source)
			continue;

		aascore.push_back(make_pair(adamic_adar(source_out, i, source), to_node[i]));
	}
}