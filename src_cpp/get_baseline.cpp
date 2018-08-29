#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <set>
#include <limits>
#include <queue>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <sqlite3.h>
#include <unordered_map>

using namespace std;
#define intt int64_t
#define ii pair<intt,intt> 
#define di pair<double, string>

vector< vector<intt> > adj;
vector< vector<intt> > in;
unordered_map<string,intt> to_indices; // unordered_maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
unordered_map<intt,string> to_node; // reverse map tp obtain node number using it's index
unordered_map<string, intt> to_ind;
unordered_map<intt, string> to_no;

sqlite3 *database;
sqlite3_stmt *res;
char *err_msg = 0;

ifstream iFile;
ifstream tsFile;
ofstream oFile;

vector<string> just_getNhop_database(string source, intt hop) //,
																				   // map<string, intt> &to_ind, map<intt, string> &to_no)
{

	char *psql = "SELECT DISTINCT ID_b FROM nodes WHERE ID_a = ? AND hop <= ?;";

	int rc = sqlite3_prepare_v2(database, psql, -1, &res, 0);

	char value1[source.size()];

	if (rc == SQLITE_OK)
	{
		strcpy(value1, source.c_str());

		sqlite3_bind_text(res, 1, value1, strlen(value1), 0);
		sqlite3_bind_int(res, 2, hop);
	}
	else
	{

		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(database));
	}

	vector<string> nhood_list;

	rc = sqlite3_step(res);
	int ncols = sqlite3_column_count(res);

	while (rc == SQLITE_ROW)
	{

		nhood_list.push_back(string((char *)sqlite3_column_text(res, 0)));
		rc = sqlite3_step(res);
	}

	sqlite3_finalize(res);

	// cout << "SQl nbd list\n";
	// for (int i = 0; i < nhood_list.size(); i++)
	// {
	// 	cout << nhood_list[i] << '\n';
	// }
	return nhood_list;
}

intt input(unordered_map<string,intt>& to_indices, unordered_map<intt,string>& to_node){
	string u; 
	intt edges = 0;
	int num_sources = 0;
	intt i = 1;
	in.resize(1);
	adj.resize(1);
			
	string source, dest;

	while(true){
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
				source = un;
				if(to_indices.find(source) == to_indices.end()){
					to_indices[source] = i;
					to_node[i] = source;
					vector< intt > tmp;
					in.push_back(tmp);
					adj.push_back(tmp);
					i++;
				}

			}

			else{
				edges++;
				double weight;
				dest = u;

				if(to_indices.find(dest) == to_indices.end()){
					to_indices[dest] = i;
					to_node[i] = dest;
					vector< intt > tmp;
					in.push_back(tmp);
					adj.push_back(tmp);
					i++;
				}
				iFile >> weight;
				// cout << source << " " << dest << "\n";
				adj[to_indices[source]].push_back(to_indices[dest]);
				in[to_indices[dest]].push_back(to_indices[source]); 

				// add reverse edge if it doens't already exist
				// if(find(in[to_indices[dest]].begin(), in[to_indices[dest]].end(), make_pair(weight, to_indices[source])) == in[to_indices[dest]].end())
				// 	in[to_indices[dest]].push_back(make_pair(weight, to_indices[source]));
			}	
		}
	}
	// cout << "Edges: " << edges << '\n';
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
vector<int> getCommon(set<int> source_node, intt v, int type){
	vector<int> common(1000000); 
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

	if(aa == 0.0)
		return -1.0;

	return aa;
}

double milne_witten(set<int> source_in, int dest, int source, int num_nodes){

	// get N'(source) intersect N'(dest)
	vector<int> common = getCommon(source_in, dest, 1);


	if(common.size() == 0)
		return -1.0;

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

	if(argc != 6){
		cout << "[Usage]: " << "./baseline trainFile testFile databaseFile outputFilename hop\n";
		return 0;
	}

	string in_file = argv[1];
	string test_file = argv[2];
	string database_loc = argv[3];
	string out_file = argv[4];
	intt hop = atoi(argv[5]);


	iFile.open(in_file);
	tsFile.open(test_file);
	oFile.open(out_file);
	// oFile.close();
	// iFile.close();
	// tsFile.close();

	int num_nodes = input(to_indices, to_node);
	char database_loc_proper[database_loc.size()];
	strcpy(database_loc_proper, database_loc.c_str());
	int rc = sqlite3_open(database_loc_proper, &database);

	while(true)
	{
		string  n1, n2;
		tsFile >> n1 >> n2;

		if(tsFile.eof()){
			break;
		}
		else
		{

			intt source = to_indices[n1];
			intt dest = to_indices[n2];

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

			int aaRank, mwRank; 
			vector<string> nbds = just_getNhop_database(n1, hop);

			if(find(nbds.begin(), nbds.end(), n2) == nbds.end()){
				aaRank = -1;
				mwRank = -1;
			}
			else
			{
				vector< di > aascore; // (rating, node)
				vector< di > mwscore; // (rating, node)

				// for every destination vertex calculate both scores and push them in an array
				// cout << n1 << ' ' << n2 << '\n';
				for(int i = 0; i < nbds.size(); i++){
					// cout << to_indices[nbds[i]] << ' ' << source << ' ' << nbds[i] << '\n';
					aascore.push_back(make_pair(adamic_adar(source_out, to_indices[nbds[i]], source), nbds[i]));
					mwscore.push_back(make_pair(milne_witten(source_in, to_indices[nbds[i]], source, num_nodes), nbds[i]));
				}

				sort(aascore.rbegin(), aascore.rend());
				sort(mwscore.rbegin(), mwscore.rend());

				// cout << "\nhere307\n";
				bool aaflag = true, mwflag = true;

				for(int i = 1; i <= aascore.size(); i++){
					if(aaflag and aascore[i-1].second == n2){
						if(aascore[i-1].first == -1.0)
							aaRank = -2;
						else
							aaRank = i;
						aaflag = false;
					}
					if(mwflag and mwscore[i-1].second == n2){
						if(mwscore[i-1].first == -1.0)
							mwRank = -2;
						else
							mwRank = i;
						mwflag = false;
					}
					if(!mwflag and !aaflag)
						break;
				}

			}
			cout << n1 << " " << n2 << " " << aaRank << " " << mwRank << '\n';
			oFile << n1 << " " << n2 << " " << aaRank << " " << mwRank << '\n';
			to_ind.clear();
			to_no.clear();
		}
	}
	oFile.close();
	iFile.close();
	tsFile.close();
	sqlite3_close(database);

	return 0;
}



