#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <map>
#include <set>
#include <limits>
#include <queue>
#include <algorithm>
#include <cmath>

using namespace std;
#define intt int64_t
#define ii pair<intt,intt> 
#define di pair<double, intt>

const intt N = 3e7;
vector< intt > adj[N];
vector< intt > in[N];
map<intt,intt> to_indices; // maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
map<intt,intt> to_node; // reverse map tp obtain node number using it's index
vector<intt> connections(N); // connecntions[i]-> number of connections of node indexed i
vector<intt> indices; // indices of connected vertices
vector<intt> weight; // distance values of connected vertices

ifstream iFile;
ofstream oFile;

intt input(map<intt,intt>& to_indices, map<intt,intt>& to_node){
	string u; 
	intt i = 1;
	while(true){
		intt source, dest;
		string u;
		// cin >> u;
		iFile >> u; // read from file

		if(iFile.eof()){
			break; // break it end of file
		}

		else{

			// check for the source node
			if(u[u.length()-1] == ':'){ 
				string un;
				un = u.substr(0, u.length()-1); // remove colon
				source = stoi(un);
				if(to_indices.find(source) == to_indices.end()){
					to_indices[source] = i;
					to_node[i] = source;
					i++;
				}
			}

			else{
				intt weight;
				dest = stoi(u);

				if(to_indices.find(dest) == to_indices.end()){
					to_indices[dest] = i;
					to_node[i] = dest;
					i++;
				}
				adj[to_indices[source]].push_back(to_indices[dest]);
				in[to_indices[dest]].push_back(to_indices[source]);
			}	
		}
	}
	return i-1; // number of distinct nodes
}


double getNbrCount(int u, string type){

	if(type == "undirected" or type == "directed_out")
		return (double)(adj[u].size());
	else if(type == "directed_in")
		return (double)(in[u].size());
	return 0.0;
}

// get common nodes between u and v. If type == 0, this method will return N(u) intersect N(v), else it will return N'(u) intersect N'(v). For undirected graph pass type = 0
vector<int> getCommon(set<int> source_node, int v, int type){
	vector<int> common(10); 
	set<int> dest_node;

	if(type == 0){
		for(int i = 0; i < adj[v].size(); i++){
			dest_node.insert(adj[v][i]);
		}
	}
	else{
		for(int i = 0; i < in[v].size(); i++){
			dest_node.insert(in[v][i]);
		}
	}
	
	vector<int>::iterator it = set_intersection(dest_node.begin(), dest_node.end(), source_node.begin(), source_node.end(), common.begin());
	common.resize(it-common.begin());

	return common;
}

/* adamic_adar(u,v) is defined as summation(1/log(|N'(x)|)) for every x that belongs to N(u) intersect N(v), where N(w) is set of nodes which have an edge from w and N'(w)
is set of nodes which have an edge towards w.*/

double adamic_adar(set<int> source_out, int dest, int source){

	// get N(source) intersect N(dest)
	vector<int> common = getCommon(source_out, dest, 0);

	double aa = 0.0;

	for(vector<int>::iterator it = common.begin(); it != common.end(); it++){
		int node = *it;

		double indeg = getNbrCount(node, (string)"directed_in");

		if(indeg == 0.0 or indeg == 1.0)
			continue;
		else{
			aa += (1.0/log((indeg)));
		}
	}

	return aa;
}

double milne_witten(set<int> source_in, int dest, int source, int num_nodes){

	// get N'(source) intersect N'(dest)
	vector<int> common = getCommon(source_in, dest, 1);

	if(common.size() == 0)
		return -1.0 * INT_MAX;

	double mw = 0.0;

	double Uindeg = getNbrCount(source, "directed_in");
	double Vindeg = getNbrCount(dest, "directed_in");

	// cout << to_node[dest] << ' ' << Vindeg << '\n';

	double nmw = (log((double)max(Uindeg, Vindeg))) - log((double)common.size());
	double dmw = (log((double)num_nodes) - log((double)min(Uindeg, Vindeg)));

	mw = nmw/dmw;

	return mw;
}	

int main(int argc, char* argv[]){

	if(argc != 4 and argc != 5){
		cout << "[Usage]: " << "./baseline input_filename output_filename source_node\n";
		cout << "[Usage]: " << "./baseline input_filename output_filename source_node node\n";
		return 0;
	}

	string in_file = argv[1];
	string out_file = argv[2];
	int source = atoi(argv[3]);
	int node;
	if(argc == 5)
		node = atoi(argv[4]);

	iFile.open(in_file);

	int num_nodes = input(to_indices, to_node);

	source = to_indices[source];
	node = to_indices[node];

	// get set of nodes with an incoming edge from source
	set<int> source_out;
	for(int i = 0; i < adj[source].size(); i++){
		source_out.insert(adj[source][i]);
	}

	// get set of nodes with outgoing edge to source
	set<int> source_in;
	for(int i = 0; i < in[source].size(); i++){
		source_in.insert(in[source][i]);
	}

	if(argc == 5){
		cout << adamic_adar(source_out, node, source) << '\n';
		cout << milne_witten(source_in, node, source, num_nodes) << '\n';
		return 0;
	}
	oFile.open(out_file);

	vector< di > aascore; // (rating, node)
	vector< di > mwscore; // (rating, node)

	// for every destination vertex calculate both scores and push them in an array
	for(int i = 1; i <= num_nodes; i++){
		if(i == source)
			continue;

		aascore.push_back(make_pair(adamic_adar(source_out, i, source), to_node[i]));
		mwscore.push_back(make_pair(milne_witten(source_in, i, source, num_nodes), to_node[i]));
	}

	sort(aascore.rbegin(), aascore.rend());
	sort(mwscore.rbegin(), mwscore.rend());

	oFile << "Adamic Adar ratings, Source = " << to_node[source] << '\n';

	for(int i = 0; i < aascore.size(); i++){
		oFile << aascore[i].second << "  " << aascore[i].first << '\n';
	}

	oFile << '\n';

	oFile << "Milne Witten ratings, Source = " << to_node[source] << '\n';

	for(int i = 0; i < mwscore.size(); i++){
		oFile << mwscore[i].second << "  " << mwscore[i].first << '\n';
	}

	oFile << '\n';
}



