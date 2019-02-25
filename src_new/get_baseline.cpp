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
#define ii pair<double,intt>
#define di pair<double, string>
#define iis pair<intt, string>

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

intt input(string train_set, bool weighted, bool directed)
{
        ifstream iFile;
        iFile.open(train_set);
        intt edges = 0;
        int num_sources = 0;
        intt i = 1;
        adj.resize(1);
        in.resize(1);
        string source, dest;
		// cout << "here\92\n";
        while (true)
        {
                iFile >> source; // read from file

                if (iFile.eof())
                {
                        break; // break it end of file
                }

                else
                {
                        iFile >> dest;

                        edges++;
                        double weight = 1.0;

                        if(weighted)
                                iFile >> weight;

                        // converting source node id to a index
                        if(to_indices.find(source) == to_indices.end())
                        {
                                to_indices[source] = i;
                                to_node[i] = source;
                                vector<intt> tmp;
                                adj.push_back(tmp);
                                in.push_back(tmp);
                                i++;
                        }

                        // converting destination node id to a index
                        if (to_indices.find(dest) == to_indices.end())
                        {
                                to_indices[dest] = i;
                                to_node[i] = dest;
                                vector<intt> tmp;
                                adj.push_back(tmp);
                                in.push_back(tmp);
                                i++;
                        }

                        adj[to_indices[source]].push_back(to_indices[dest]);
                        in[to_indices[dest]].push_back(to_indices[source]);

						if(!directed){
							adj[to_indices[dest]].push_back(to_indices[source]);
	                        in[to_indices[source]].push_back(to_indices[dest]);
						}
                }
        }
        // cout << "Edges: " << edges << '\n';
        return i - 1; // number of distinct nodes
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

int getCommon(set<int> source_node, set<int> dest_node){
	vector<int> common(1000000);

	vector<int>::iterator it = set_intersection(dest_node.begin(), dest_node.end(), source_node.begin(), source_node.end(), common.begin());
	common.resize(it-common.begin());

	return common.size();
}

set<int> getNbrhood(intt source){
	set<int> source_out;
	for(int i = 0; i < adj[source].size(); i++){
		source_out.insert(adj[source][i]);
	}
	return source_out;
}

/* adamic_adar(u,v) is defined as summation(1/log(|N'(x)|)) for every x that belongs to N(u) intersect N(v), where N(w) is set of nodes which have an edge from w and N'(w)
is set of nodes which have an edge towards w.*/

double adamic_adar(set<int> source_out, int dest, int source, string type){

	// get N(source) intersect N(dest)
	vector<int> common = getCommon(source_out, dest, 0);

	double aa = 0.0;

	for(vector<int>::iterator it = common.begin(); it != common.end(); it++){
		int node = *it;

		double indeg = getNbrCount(node, type);

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

double milne_witten(set<int> source_in, int dest, int source, int num_nodes, string type){

	// get N'(source) intersect N'(dest)
	vector<int> common = getCommon(source_in, dest, 1);


	if(common.size() == 0)
		return -1.0;

	double mw = 0.0;

	double Uindeg = getNbrCount(source, type);
	double Vindeg = getNbrCount(dest, type);

	// cout << to_node[dest] << ' ' << Vindeg << '\n';

	double nmw = (log((double)max(Uindeg, Vindeg))) - log((double)common.size());
	double dmw = (log((double)num_nodes) - log((double)min(Uindeg, Vindeg)));

	mw = nmw/dmw;

	return mw;
}

int main(int argc, char* argv[]){

	if(argc < 5){
		cout << "[Usage]: " << "./baseline --directed --weighted trainFile testFile outputFilename hop\n";
		return 0;
	}

	int j = 1;

	bool directed = false;
	bool weighted = false;

	if((string)argv[1] == "--directed"){
		j++;
		directed = true;
	}

	if((string)argv[2] == "--weighted"){
		j++;
		weighted = true;
	}

	string in_file = argv[j++];
	string test_file = argv[j++];
	// string database_loc = argv[j++];
	string out_file = argv[j++];
	intt hop = atoi(argv[j++]);


	iFile.open(in_file);
	tsFile.open(test_file);
	oFile.open(out_file);
	// oFile.close();
	// iFile.close();
	// tsFile.close();

	int num_nodes = input(in_file, weighted, directed);

	string type = "undirected";
	if(directed)
		type = "directed_in";

	// char database_loc_proper[database_loc.size()];
	// strcpy(database_loc_proper, database_loc.c_str());
	// int rc = sqlite3_open(database_loc_proper, &database);

	vector<set<int> > neighborhoods(num_nodes+1);

	for(int i = 1; i <= num_nodes; i++){
		neighborhoods[i] = getNbrhood(i);
	}

	while(true)
	{
		string  n1, n2;
		tsFile >> n1 >> n2;
		double weight = 1.0;

		if(tsFile.eof()){
			break;
		}

		else
		{
			if(weighted)
				iFile >> weight;

			intt source = to_indices[n1];
			intt dest = to_indices[n2];

			// int aaRank, mwRank;
			int paRank, cmRank;
		
			vector< iis > pascore; // (rating, node)
			vector< iis > cmscore; // (rating, node)

			// for every destination vertex calculate both scores and push them in an array
			// cout << n1 << ' ' << n2 << '\n';
			for(int i = 1; i <= num_nodes; i++){
				if(i == source)
					continue;

				intt pa = neighborhoods[source].size() * neighborhoods[i].size();
				intt cm = getCommon(neighborhoods[source], neighborhoods[i]);

				// cout << n1 << ' ' << to_node[i] << ' ' << pa << ' ' << cm << '\n';

				pascore.push_back(make_pair(pa, to_node[i]));
				cmscore.push_back(make_pair(cm, to_node[i]));
			}

			// sort(aascore.rbegin(), aascore.rend());
			// sort(mwscore.rbegin(), mwscore.rend());

			sort(pascore.rbegin(), pascore.rend());
			sort(cmscore.rbegin(), cmscore.rend());

			// cout << "\nhere307\n";
			// bool aaflag = true, mwflag = true;

			bool paflag = true, cmflag = true;

			for(int i = 1; i <= pascore.size(); i++){
				if(paflag and pascore[i-1].second == n2){
					if(pascore[i-1].first == 0)
						paRank = num_nodes-1;
					else
						paRank = i;
					paflag = false;
				}
				if(cmflag and cmscore[i-1].second == n2){
					if(cmscore[i-1].first == 0)
						cmRank = num_nodes-1;
					else
						cmRank = i;
					cmflag = false;
				}
				if(!cmflag and !paflag)
					break;
			}

			cout << n1 << " " << n2 << " " << paRank << " " << cmRank << '\n';
			oFile << n1 << " " << n2 << " " << paRank << " " << cmRank << '\n';
			to_ind.clear();
			to_no.clear();
		}
	}
	oFile.close();
	iFile.close();
	tsFile.close();
	// sqlite3_close(database);

	return 0;
}


// int main(int argc, char* argv[]){

// 	if(argc < 5){
// 		cout << "[Usage]: " << "./baseline --directed --weighted trainFile testFile databaseFile outputFilename hop\n";
// 		return 0;
// 	}

// 	int j = 1;

// 	bool directed = false;
// 	bool weighted = false;

// 	if((string)argv[1] == "--directed"){
// 		j++;
// 		directed = true;
// 	}

// 	if((string)argv[2] == "--weighted"){
// 		j++;
// 		weighted = true;
// 	}

// 	string in_file = argv[j++];
// 	string test_file = argv[j++];
// 	string database_loc = argv[j++];
// 	string out_file = argv[j++];
// 	intt hop = atoi(argv[j++]);


// 	iFile.open(in_file);
// 	tsFile.open(test_file);
// 	oFile.open(out_file);
// 	// oFile.close();
// 	// iFile.close();
// 	// tsFile.close();

// 	int num_nodes = input(in_file, weighted, directed);

// 	string type = "undirected";
// 	if(directed)
// 		type = "directed_in";

// 	char database_loc_proper[database_loc.size()];
// 	strcpy(database_loc_proper, database_loc.c_str());
// 	int rc = sqlite3_open(database_loc_proper, &database);

// 	while(true)
// 	{
// 		string  n1, n2;
// 		tsFile >> n1 >> n2;
// 		double weight = 1.0;

// 		if(tsFile.eof()){
// 			break;
// 		}

// 		else
// 		{
// 			if(weighted)
// 				iFile >> weight;

// 			intt source = to_indices[n1];
// 			intt dest = to_indices[n2];

// 			// get set of nodes with an incoming edge from source
// 			set<int> source_out;
// 			for(int i = 0; i < adj[source].size(); i++){
// 				source_out.insert(adj[source][i]);
// 			}

// 			// get set of nodes with outgoing edge to source
// 			set<int> source_in;
// 			for(int i = 0; i < in[source].size(); i++){
// 				source_in.insert(in[source][i]);
// 			}

// 			// int aaRank, mwRank;
// 			int paRank, cmRank;
// 			vector<string> nbds = just_getNhop_database(n1, hop);

// 			if(find(nbds.begin(), nbds.end(), n2) == nbds.end()){
// 				// aaRank = -1;
// 				// mwRank = -1;
// 				paRank = -1;
// 				cmRank = -1;
// 			}
// 			else
// 			{
// 				// vector< di > aascore; // (rating, node)
// 				// vector< di > mwscore; // (rating, node)
// 				vector< di > pascore; // (rating, node)
// 				vector< di > cmscore; // (rating, node)


// 				// for every destination vertex calculate both scores and push them in an array
// 				// cout << n1 << ' ' << n2 << '\n';
// 				for(int i = 0; i < nbds.size(); i++){
// 					// cout << to_indices[nbds[i]] << ' ' << source << ' ' << nbds[i] << '\n';
// 					// aascore.push_back(make_pair(adamic_adar(source_out, to_indices[nbds[i]], source, type), nbds[i]));
// 					// mwscore.push_back(make_pair(milne_witten(source_in, to_indices[nbds[i]], source, num_nodes, type), nbds[i]));
// 					pascore.push_back(make_pair(pa(source_out, to_indices[nbds[i]], source), nbds[i]));
// 					cmscore.push_back(make_pair((double)getCommon(source_out, dest, 0).size(), nbds[i]));
// 				}

// 				// sort(aascore.rbegin(), aascore.rend());
// 				// sort(mwscore.rbegin(), mwscore.rend());

// 				sort(pascore.rbegin(), pascore.rend());
// 				sort(cmscore.rbegin(), cmscore.rend());

// 				// cout << "\nhere307\n";
// 				// bool aaflag = true, mwflag = true;

// 				bool paflag = true, cmflag = true;

// 				for(int i = 1; i <= pascore.size(); i++){
// 					if(paflag and pascore[i-1].second == n2){
// 						if(pascore[i-1].first == -1.0)
// 							paRank = -2;
// 						else
// 							paRank = i;
// 						paflag = false;
// 					}
// 					if(cmflag and cmscore[i-1].second == n2){
// 						if(cmscore[i-1].first == -1.0)
// 							cmRank = -2;
// 						else
// 							cmRank = i;
// 						cmflag = false;
// 					}
// 					if(!cmflag and !paflag)
// 						break;
// 				}

// 			}
// 			cout << n1 << " " << n2 << " " << paRank << " " << cmRank << '\n';
// 			oFile << n1 << " " << n2 << " " << paRank << " " << cmRank << '\n';
// 			to_ind.clear();
// 			to_no.clear();
// 		}
// 	}
// 	oFile.close();
// 	iFile.close();
// 	tsFile.close();
// 	sqlite3_close(database);

// 	return 0;
// }

