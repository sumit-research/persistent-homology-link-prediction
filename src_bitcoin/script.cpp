#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <limits>
#include <queue>
#include <sqlite3.h>
#include "apsp.h"
#include "nhop.h"
#include "./ripser/ripser_header.h"
#include "./bottleneck/bottleneck.h"
#include "./wasserstein/wasserstein.h"

using namespace std;
#define intt int64_t
#define ii pair<double, intt>
#define fastio                    \
	ios_base::sync_with_stdio(0); \
	cin.tie(0);                   \
	cout.tie(0)

vector<vector<ii>> in;
vector<vector<ii>> reverse_in;
unordered_map<string, intt> to_ind;
unordered_map<intt, string> to_no;
unordered_map<intt, intt> to_indices_nhop;
unordered_map<intt, intt> to_node_nhop;

unordered_map<string, intt> to_indices; // maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
unordered_map<intt, string> to_node;	// reverse map to obtain node number using it's index

// SQL parameters
sqlite3 *database;
sqlite3_stmt *res;
char *err_msg = 0;

ifstream tsFile;
ofstream oFile;
vector<string> just_getNhop_database(string dataset_name, string source, intt hop) //,
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

vector<vector<ii>> getNhop_database(string dataset_name, vector<string> sources, intt hop) //,
																						   // map<string, intt> &to_ind, map<intt, string> &to_no)
{

	char *psql = "WITH nhood1 as (SELECT ID_b FROM nodes WHERE ID_a = ? and hop <= ? UNION Select ID_b FROM nodes where ID_a = ? AND hop <= ?) Select * FROM nodes Where ID_a IN nhood1 AND ID_B IN nhood1 AND HOP<=1;";

	int rc = sqlite3_prepare_v2(database, psql, -1, &res, 0);

	char value1[sources[0].size()];
	char value3[sources[1].size()];

	if (rc == SQLITE_OK)
	{
		strcpy(value1, sources[0].c_str());
		strcpy(value3, sources[1].c_str());

		sqlite3_bind_text(res, 1, value1, strlen(value1), 0);
		sqlite3_bind_int(res, 2, hop);
		sqlite3_bind_text(res, 3, value3, strlen(value3), 0);
		sqlite3_bind_int(res, 4, hop);
	}
	else
	{

		fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(database));
	}

	vector<vector<ii>> nhood_graph;

	rc = sqlite3_step(res);
	int ncols = sqlite3_column_count(res);
	intt current_node = 1;
	nhood_graph.resize(1);

	while (rc == SQLITE_ROW)
	{

		vector<string> temp(ncols);

		for (int i = 0; i < ncols; i++)
		{
			temp[i] = string((char *)sqlite3_column_text(res, i));
		}

		if (to_ind.find(temp[0]) == to_ind.end())
		{
			to_ind[temp[0]] = current_node;
			to_no[current_node] = temp[0];
			vector<ii> tmp;
			nhood_graph.push_back(tmp);
			current_node++;
		}
		if (to_ind.find(temp[1]) == to_ind.end())
		{
			to_ind[temp[1]] = current_node;
			to_no[current_node] = temp[1];
			vector<ii> tmp;
			nhood_graph.push_back(tmp);
			current_node++;
		}

		if (temp[2] != "0")
			nhood_graph[to_ind[temp[0]]].push_back(make_pair(1, to_ind[temp[1]]));
		// results.push_back(temp);

		rc = sqlite3_step(res);
	}

	sqlite3_finalize(res);
	//   sqlite3_close(database);

	// cout << "With SQL\n";
	// for (int i = 1; i < nhood_graph.size(); i++)
	// {
	// 	cout << to_no[i] << "-> ";
	// 	for (int j = 0; j < nhood_graph[i].size(); j++)
	// 		cout << to_no[nhood_graph[i][j].second] << " ";

	// 	cout << '\n';
	// }
	return nhood_graph;
}

vector<vector<ii>> getNHop(vector<vector<ii>> &graph, vector<string> sources, intt hop) //,
																						//    map<intt, intt> &to_indices_nhop, map<intt, intt> &to_node_nhop)
{
	// Add these 3 lines
	vector<intt> intt_sources;
	intt_sources.push_back(to_indices[sources[0]]);
	intt_sources.push_back(to_indices[sources[1]]);
	vector<intt> dist = bfs(graph, intt_sources, graph.size() - 1);

	vector<intt> nbd;
	vector<vector<ii>> new_graph;
	intt current_node = 1;

	new_graph.resize(1);
	for (intt v = 1; v < dist.size(); v++)
	{
		if (dist[v] <= hop)
		{
			if (to_indices_nhop.find(v) == to_indices_nhop.end())
			{
				to_indices_nhop[v] = current_node;
				to_node_nhop[current_node] = v;
				vector<ii> tmp;
				new_graph.push_back(tmp);
				current_node++;
			}

			intt source = to_indices_nhop[v];
			// graph -> v and new_graph -> source

			for (int i = 0; i < graph[v].size(); i++)
			{

				if (dist[graph[v][i].second] <= hop)
				{
					intt dest = graph[v][i].second;
					if (to_indices_nhop.find(dest) == to_indices_nhop.end())
					{
						to_indices_nhop[dest] = current_node;
						to_node_nhop[current_node] = dest;
						vector<ii> tmp;
						new_graph.push_back(tmp);
						current_node++;
					}

					// cout << graph[v][i].first << " " << to_indices_nhop[dest] << '\n';
					new_graph[source].push_back(make_pair(graph[v][i].first /* weight */, to_indices_nhop[dest] /*node_id*/));
				}
			}
		}
	}

	return new_graph;
}

vector<vector<ii>> addEdge(vector<vector<ii>> graph, intt a, intt b)
{
	vector<vector<ii>> empty_graph = graph;
	empty_graph[a].push_back(make_pair(0.142611469, b));
	return empty_graph;
}

vector<vector<double>> getPD(intt graph_size)
{
	vector<vector<double>> apsp;
	for (intt i = 0; i < graph_size; i++)
	{
		apsp.push_back(vector<double>(graph_size + 1));
		for (intt j = 1; j <= graph_size; j++)
		{
			apsp[i][j] = 1.0;
		}
	}
	return call_ripser(1, 4, apsp); //threshold = 4 is for some reason. max dimension = 1 required by us.
}

vector<vector<double>> getPD(vector<vector<ii>> small_graph)
{
	vector<vector<ii>> reverse_graph;
	reverse_graph.resize(small_graph.size());

	for (intt i = 1; i < small_graph.size(); i++)
	{
		for (intt j = 0; j < small_graph[i].size(); j++)
		{
			reverse_graph[small_graph[i][j].second].push_back(make_pair(small_graph[i][j].first, i));
		}
	}
	vector<vector<double>> apsp = johnson(small_graph, reverse_graph, small_graph.size() - 1);
	return call_ripser(1, 4, apsp); //threshold = 4 is for some reason. max dimension = 1 required by us.
}
vector<pair<double, double>> getDimPD(vector<vector<double>> pd, double dimension)
{
	vector<pair<double, double>> pdgm;
	for (size_t i = 0; i < pd.size(); i++)
	{
		if (pd[i][0] == dimension)
		{
			pdgm.push_back({pd[i][1], pd[i][2]});
		}
	}
	return pdgm;
}

vector<double> callFunctions(vector<string> sources, intt hop, string dataset_name)
{

	to_ind.clear();
	to_no.clear();
	to_indices_nhop.clear();
	to_node_nhop.clear();
	vector<vector<ii>> comb_nbd = getNhop_database(dataset_name, sources, hop); //, to_ind, to_no);
	// vector<vector<ii>> comb_nbd = getNHop(in, sources, hop); //, to_indices_nhop, to_node_nhop); //_without_database
	vector<vector<ii>> comb_nbd_with_edge = addEdge(comb_nbd, to_ind[sources[0]], to_ind[sources[1]]);
	// vector<vector<ii>> comb_nbd_with_edge = addEdge(comb_nbd, to_indices_nhop[to_indices[sources[0]]], to_indices_nhop[to_indices[sources[1]]]);

	vector<vector<double>> pd_ab = getPD(comb_nbd);
	vector<vector<double>> pd_ab_with_edge = getPD(comb_nbd_with_edge);
	vector<vector<double>> pd_ab_complete = getPD(comb_nbd.size() - 1);
	vector<pair<double, double>> pdgm = getDimPD(pd_ab, double(0));
	vector<pair<double, double>> pdgm_with_edge = getDimPD(pd_ab_with_edge, double(0));
	vector<pair<double, double>> pdgm_complete = getDimPD(pd_ab_complete, double(0));

	//The parameter for w_distance computation are the one specified there as default (mostly).
	hera::AuctionParams<double> params;
	params.wasserstein_power = 2.0; //for wasserstein_power = 1.0 it removes duplicates, to finalize this one.
	params.delta = 0.01;
	params.internal_p = hera::get_infinity<double>(); //taking l-infinity metric here.
	if (std::isinf(params.internal_p))
	{
		params.internal_p = hera::get_infinity<double>();
	}
	params.initial_epsilon = 0.0;
	params.epsilon_common_ratio = 0.0;
	params.max_bids_per_round = 0;
	if (params.max_bids_per_round == 0)
		params.max_bids_per_round = std::numeric_limits<size_t>::max();
	params.gamma_threshold = 0.0;
	params.max_num_phases = 800;

	vector<double> output;
	double bn_dist_0 = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
	output.push_back(bn_dist_0);
	double bn_dist_complete_0 = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
	output.push_back(bn_dist_complete_0);
	double w_dist_0 = hera::wasserstein_dist(pdgm, pdgm_with_edge, params, "");
	output.push_back(w_dist_0);
	double w_dist_complete_0 = hera::wasserstein_dist(pdgm_complete, pdgm_with_edge, params, "");
	output.push_back(w_dist_complete_0);

	pdgm = getDimPD(pd_ab, double(1));
	pdgm_with_edge = getDimPD(pd_ab_with_edge, double(1));
	pdgm_complete = getDimPD(pd_ab_complete, double(1));

	double bn_dist_1 = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
	output.push_back(bn_dist_1);
	double bn_dist_complete_1 = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
	output.push_back(bn_dist_complete_1);
	double w_dist_1 = hera::wasserstein_dist(pdgm, pdgm_with_edge, params, "");
	output.push_back(w_dist_1);
	double w_dist_complete_1 = hera::wasserstein_dist(pdgm_complete, pdgm_with_edge, params, "");
	output.push_back(w_dist_complete_1);

	// cout << "\nbn_dist_dim0=" << bn_dist_0 << "\tbn_dist_dim1=" << bn_dist_1;
	// cout << "\tbn_dist_comp_dim0=" << bn_dist_complete_0 << "\tbn_dist_comp_dim1=" << bn_dist_complete_1 << "\n";
	// cout << "\nw_dist_dim0=" << w_dist_0 << "\tw_dist_dim1=" << w_dist_1;
	// cout << "\tw_dist_comp_dim0=" << w_dist_complete_0 << "\tw_dist_comp_dim1=" << w_dist_complete_1 << "\n";
	return output;
}

int main(int argc, char *argv[])
{
	fastio;

	if (argc < 6)
	{
		cout << "[Usage]: "
			 << "./script dataset_name train_set test_set nbd_hop comb_nbd_hop output_file\n";
		return 0;
	}

	string dataset_name = argv[1];
	string train_set = argv[2];
	string test_set = argv[3];
	int nbd_hop = atoi(argv[4]);
	int comb_nbd_hop = atoi(argv[5]);
	string output_file = argv[6];

	string database_loc = "/home/deepak/Project/files/outputs/bitcoin/database.db";
	char database_loc_proper[database_loc.size()];
	strcpy(database_loc_proper, database_loc.c_str());
	int rc = sqlite3_open(database_loc_proper, &database);

	tsFile.open((string)test_set);
	oFile.open((string)output_file);
	// sFile.open((string) sparse_file, ios::binary);

	intt num_nodes = input(train_set, in, reverse_in, to_indices, to_node); // take input
	if (tsFile)
	{
		while (true)
		{
			string source, dest;
			// cin >> u;
			tsFile >> source >> dest; // read from file

			if (tsFile.eof())
			{
				break; // break it end of file
			}

			else
			{
				vector<string> src_nbd = just_getNhop_database(dataset_name, source, nbd_hop);
				bool isFoundDest = false;
				// cout<<"\nSource: "<<source<<"\t NbdSize = "<<src_nbd.size()<<"\n";
				// int a;
				// cin>>a;
				for (size_t i = 0; i < src_nbd.size(); i++)
				{
					// cout << "\n"
					// 	 << i << "," << src_nbd[i] << "\n";

					if (src_nbd[i] == dest)
					{
						isFoundDest = true;
						break;
					}
				}
				// cout<<"\nisFound: "<<isFoundDest<<"\n";
				if (isFoundDest)
				{

					for (size_t i = 0; i < src_nbd.size(); i++)
					{
						vector<string> sources;
						sources.push_back(source);
						sources.push_back(src_nbd[i]);
						// cout << "\n"<<source << "\t" << src_nbd[i] << "\n";
						vector<double> scores = callFunctions(sources, comb_nbd_hop, dataset_name);
						cout << "\n"
							 << source << "\t"
							 << dest << "\t" 
							 << src_nbd[i] << ":"
							 << scores[0] << ","
							 << scores[1] << ","
							 << scores[2] << ","
							 << scores[3] << ","
							 << scores[4] << ","
							 << scores[5] << ","
							 << scores[6] << ","
							 << scores[7] << "\t";
						oFile << "\n"
							  << source << "\t"
							  << dest << "\t"
							  << src_nbd[i] << ":"
							  << scores[0] << ","
							  << scores[1] << ","
							  << scores[2] << ","
							  << scores[3] << ","
							  << scores[4] << ","
							  << scores[5] << ","
							  << scores[6] << ","
							  << scores[7] << "\t";
						scores.clear();
					}
				}
				else
				{
					cout << "\n"
						 << source << "\t" << dest << "\t"
						 << "inf";
					oFile << "\n"
						  << source << "\t" << dest << "\t"
						  << "inf";
				}

				src_nbd.clear();
				// break;
			}
		}
	}

	tsFile.close();
	oFile.close();
	sqlite3_close(database);

	return 0;
}

// //////////////////////////////////Working one below//////////////////////////////////////////////////
// #include <iostream>
// #include <string>
// #include <fstream>
// #include <vector>
// #include <utility>
// #include <queue>
// #include <tuple>
// #include <map>
// #include <limits>
// #include <queue>
// #include "apsp.h"
// #include "nhop.h"
// #include "./ripser/ripser_header.h"
// #include "./bottleneck/bottleneck.h"

// using namespace std;
// #define intt int64_t
// #define ii pair<intt, intt>
// #define fastio                    \
// 	ios_base::sync_with_stdio(0); \
// 	cin.tie(0);                   \
// 	cout.tie(0)

// vector<vector<ii>> in;
// vector<vector<ii>> reverse_in;

// map<intt, intt> to_indices_nhop;
// map<intt, intt> to_node_nhop;

// ifstream tsFile;
// ofstream oFile;

// vector<vector<ii>> getNHop(vector<vector<ii>> &graph, vector<intt> sources, intt hop)
// {
// 	// vector<intt> sources = {to_indices[a], to_indices[b]};

// 	vector<intt> dist = bfs(graph, sources, graph.size() - 1);

// 	// cout << "BFS done\n";

// 	vector<intt> nbd;
// 	vector<vector<ii>> new_graph;
// 	intt current_node = 1;

// 	new_graph.resize(1);

// 	// cout << "here44\n";
// 	for (intt v = 1; v < graph.size(); v++)
// 	{
// 		if (dist[v] <= hop)
// 		{
// 			// cout << v << '\n';
// 			if (to_indices_nhop.find(v) == to_indices_nhop.end())
// 			{
// 				to_indices_nhop[v] = current_node;
// 				to_node_nhop[current_node] = v;
// 				vector<ii> tmp;
// 				new_graph.push_back(tmp);
// 				current_node++;
// 			}

// 			intt source = to_indices_nhop[v];
// 			// graph -> v and new_graph -> source

// 			for (int i = 0; i < graph[v].size(); i++)
// 			{

// 				if (dist[graph[v][i].second] <= hop)
// 				{
// 					intt dest = graph[v][i].second;
// 					if (to_indices_nhop.find(dest) == to_indices_nhop.end())
// 					{
// 						to_indices_nhop[dest] = current_node;
// 						to_node_nhop[current_node] = dest;
// 						vector<ii> tmp;
// 						new_graph.push_back(tmp);
// 						current_node++;
// 					}

// 					// cout << graph[v][i].first << " " << to_indices_nhop[dest] << '\n';
// 					new_graph[source].push_back(make_pair(graph[v][i].first /* weight */, to_indices_nhop[dest] /*node_id*/));
// 				}
// 			}
// 		}
// 	}

// 	return new_graph;
// }

// vector<vector<ii>> addEdge(vector<vector<ii>> graph, intt a, intt b)
// {
// 	vector<vector<ii>> empty_graph = graph;
// 	empty_graph[a].push_back(make_pair(1, b));
// 	return empty_graph;
// }

// vector<vector<double>> getPD(intt graph_size)
// {
// 	vector<vector<double>> apsp; // = johnson(small_graph, reverse_graph, small_graph.size() - 1);
// 	for (intt i = 0; i < graph_size; i++)
// 	{
// 		apsp.push_back(vector<double>(graph_size+1));
// 		for (intt j = 1; j <= graph_size; j++)
// 		{
// 			apsp[i][j] = 1.0;
// 		}
// 	}

// 	return call_ripser(1, 4, apsp); //threshold = 4 is for some reason. max dimension = 1 required by us.
// }

// vector<vector<double>> getPD(vector<vector<ii>> small_graph)
// {
// 	vector<vector<ii>> reverse_graph;
// 	reverse_graph.resize(small_graph.size());

// 	for (intt i = 1; i < small_graph.size(); i++)
// 	{
// 		for (intt j = 0; j < small_graph[i].size(); j++)
// 		{
// 			reverse_graph[small_graph[i][j].second].push_back(make_pair(small_graph[i][j].first, i));
// 		}
// 	}
// 	vector<vector<double>> apsp = johnson(small_graph, reverse_graph, small_graph.size() - 1);
// 	return call_ripser(1, 4, apsp); //threshold = 4 is for some reason. max dimension = 1 required by us.
// }
// vector<pair<double, double>> getDimPD(vector<vector<double>> pd, double dimension)
// {
// 	vector<pair<double, double>> pdgm;
// 	for (size_t i = 0; i < pd.size(); i++)
// 	{
// 		if (pd[i][0] == dimension)
// 		{
// 			pdgm.push_back({pd[i][1], pd[i][2]});
// 			// cout << "pd.size()=" << pd.size() << "\t"
// 			// 	 << "pdgm.size()=" << pdgm.size() << "\n";
// 		}
// 	}
// 	return pdgm;
// }

// void callFunctions(vector<intt> sources, int hop, map<string, intt> to_indices, map<intt, string> to_node)
// {
// 	vector<vector<ii>> comb_nbd = getNHop(in, sources, hop);
// 	vector<vector<ii>> comb_nbd_with_edge = addEdge(comb_nbd, to_indices_nhop[sources[0]], to_indices_nhop[sources[1]]);
// 	cout << "\ncomb_nbd\n";

// 	for (intt i = 1; i < comb_nbd.size(); i++)
// 	{
// 		for (intt j = 0; j < comb_nbd[i].size(); j++)
// 		{
// 			cout << "\tsource=" << i << "\t"
// 				 << "dest=" << comb_nbd[i][j].second << "\n ";
// 		}
// 	}
// 	cout << "\ncomb_nbd_with_edge\n";

// 	for (intt i = 1; i < comb_nbd_with_edge.size(); i++)
// 	{
// 		for (intt j = 0; j < comb_nbd_with_edge[i].size(); j++)
// 		{
// 			cout << "\tsource=" << i << "\t"
// 				 << "dest=" << comb_nbd_with_edge[i][j].second << "\n ";
// 		}
// 	}

// 	vector<vector<double>> pd_ab = getPD(comb_nbd);
// 	vector<vector<double>> pd_ab_with_edge = getPD(comb_nbd_with_edge);
// 	vector<vector<double>> pd_ab_complete = getPD(comb_nbd.size() - 1);
// 	cout << "\nPDPAIR\n";
// 	for (size_t i = 0; i < pd_ab.size(); i++)
// 	{

// 		for (size_t j = 0; j < pd_ab[i].size(); j++)
// 		{
// 			cout << pd_ab[i][j] << " ";
// 		}
// 		cout << '\n';
// 	}
// 	cout << "\nPDPAIR_with_edge\n";
// 	for (size_t i = 0; i < pd_ab_with_edge.size(); i++)
// 	{

// 		for (size_t j = 0; j < pd_ab_with_edge[i].size(); j++)
// 		{
// 			cout << pd_ab_with_edge[i][j] << " ";
// 		}
// 		cout << '\n';
// 	}
// 	vector<pair<double, double>> pdgm = getDimPD(pd_ab, double(0));
// 	vector<pair<double, double>> pdgm_with_edge = getDimPD(pd_ab_with_edge, double(0));
// 	vector<pair<double, double>> pdgm_complete = getDimPD(pd_ab_complete, double(0));
// 	cout << "\nPDPAIR_DIM_0\n";
// 	for (size_t i = 0; i < pdgm.size(); i++)
// 	{

// 		cout << pdgm[i].first << " " << pdgm[i].second << " ";
// 		cout << '\n';
// 	}
// 	cout << "\nPDPAIR_WITHEDGE_DIM_0\n";
// 	for (size_t i = 0; i < pdgm_with_edge.size(); i++)
// 	{

// 		cout << pdgm_with_edge[i].first << " " << pdgm_with_edge[i].second << " ";
// 		cout << '\n';
// 	}
// 	cout << "\nPDPAIR_COMPLETE_DIM_0\n";
// 	for (size_t i = 0; i < pdgm_complete.size(); i++)
// 	{

// 		cout << pdgm_complete[i].first << " " << pdgm_complete[i].second << " ";
// 		cout << '\n';
// 	}

// 	int decPrecision{0};

// 	double res0 = hera::bottleneckDistExact(pdgm, pdgm_with_edge, decPrecision);
// 	double res_complete0 = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge, decPrecision);

// 	pdgm = getDimPD(pd_ab, double(1));
// 	pdgm_with_edge = getDimPD(pd_ab_with_edge, double(1));
// 	pdgm_complete = getDimPD(pd_ab_complete, double(1));
// 	cout << "\nPDPAIR_DIM_1\n";
// 	for (size_t i = 0; i < pdgm.size(); i++)
// 	{

// 		cout << pdgm[i].first << " " << pdgm[i].second << " ";
// 		cout << '\n';
// 	}
// 	cout << "\nPDPAIR_WITHEDGE_DIM_1\n";
// 	for (size_t i = 0; i < pdgm_with_edge.size(); i++)
// 	{

// 		cout << pdgm_with_edge[i].first << " " << pdgm_with_edge[i].second << " ";
// 		cout << '\n';
// 	}
// 	cout << "\nPDPAIR_COMPLETE_DIM_1\n";
// 	for (size_t i = 0; i < pdgm_complete.size(); i++)
// 	{

// 		cout << pdgm_complete[i].first << " " << pdgm_complete[i].second << " ";
// 		cout << '\n';
// 	}

// 	double res1 = hera::bottleneckDistExact(pdgm, pdgm_with_edge, decPrecision);
// 	double res_complete1 = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge, decPrecision);

// 	cout << "\nbndist_dim0=" << res0 << "\tbndist_dim1=" << res1 << "\n";
// 	cout << "\nbndist_complete_dim0=" << res_complete0 << "\tbndist_complete_dim1=" << res_complete1 << "\n";
// }

// int main(int argc, char *argv[])
// {
// 	fastio;

// 	map<string, intt> to_indices; // maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
// 	map<intt, string> to_node;	// reverse map to obtain node number using it's index

// 	if (argc < 5)
// 	{
// 		cout << "[Usage]: "
// 			 << "./script dataset_name train_set test_set nbd_hop output_file\n";
// 		return 0;
// 	}

// 	string dataset_name = argv[1];
// 	string train_set = argv[2];
// 	string test_set = argv[3];
// 	int nbd_hop = atoi(argv[4]);
// 	string output_file = argv[5];

// 	tsFile.open((string)test_set);
// 	oFile.open((string)output_file);
// 	// sFile.open((string) sparse_file, ios::binary);

// 	intt num_nodes = input(train_set, in, reverse_in, to_indices, to_node); // take input
// 	if (tsFile)
// 	{
// 		while (true)
// 		{
// 			string source, dest;
// 			// cin >> u;
// 			tsFile >> source >> dest; // read from file

// 			if (tsFile.eof())
// 			{
// 				break; // break it end of file
// 			}

// 			else
// 			{
// 				// std::cout << nbd_hop << "\n";
// 				vector<intt> sources;
// 				// sources.push_back(to_indices[source]);
// 				// sources.push_back(to_indices[dest]);
// 				sources.push_back(to_indices["1034"]);
// 				sources.push_back(to_indices["1035"]);
// 				cout << "\nsource = " << source << "\tdest = " << dest << "\n";
// 				callFunctions(sources, nbd_hop, to_indices, to_node);
// 				break;
// 			}
// 		}
// 		tsFile.close();
// 	}

// 	oFile.close();

// 	return 0;
// }

// // // for(intt i = 1; i < small_graph.size(); i++)
// // // {
// // // 	for(intt j = 0; j < small_graph[i].size(); j++)
// // // 	{
// // // 		cout << "\tsource=" << i << "\t" << "dest=" << small_graph[i][j].second << "\n ";
// // // 	}
// // // }

// // // cout << "reverse_graph\n";
// // // for(intt i = 1; i < reverse_graph.size(); i++)
// // // {
// // // 	for(intt j = 0; j < reverse_graph[i].size(); j++)
// // // 	{
// // // 		cout << "\tsource=" << i << "\t" << "dest=" << reverse_graph[i][j].second << "\n ";
// // // 	}
// // // }
// // // cout << "here95\n";
// // // cout << small_graph.size();
// // // for(int i = 0; i < small_graph.size(); i++){
// // // 	for(int j = 1; j < small_graph[i].size(); j++){
// // // 		cout << small_graph[i][j].second << " ";
// // // 	}
// // // 	cout << '\n';
// // // }
// // // for(int i = 0; i < apsp.size(); i++){
// // // 	for(int j = 1; j < apsp[i].size(); j++){
// // // 		cout << apsp[i][j] << " ";
// // // 	}
// // // 	cout << '\n';
// // // }
// // // vector<vector<double>> empty;
// // // return empty;

// // // cout << num_nodes << "\t\n";
// // // for (intt i = 1; i < in.size(); i++)
// // // {
// // // 	for (intt j = 0; j < in[i].size(); j++)
// // // 	{
// // // 		int a;
// // // 		cout << "in.size()=" << in.size() << "\t"
// // // 			 << "in[i].size()=" << in[i].size() << "\ti=" << i << "\t"
// // // 			 << "j=" << j << "\t"
// // // 			 << "in[i][j].first=" << in[i][j].first << "\t"
// // // 			 << "in[i][j].second=" << in[i][j].second << "\n ";
// // // 		// cin>>a;
// // // 	}
// // // }
