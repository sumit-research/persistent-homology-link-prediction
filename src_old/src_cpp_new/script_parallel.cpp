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
#include <omp.h>
#include <sqlite3.h>
#include "apsp.h"
#include "nhop.h"
#include "./ripser/ripser_header.h"
#include "./bottleneck/bottleneck.h"
#include "./wasserstein/wasserstein.h"

using namespace std;
#define intt int64_t
#define ii pair<intt, intt>
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

vector<vector<ii>> getNhop_database(string dataset_name, pair<string, string> sources, intt hop) //,
																								 // map<string, intt> &to_ind, map<intt, string> &to_no)
{

	char *psql = "SELECT DISTINCT n_hood1.ID_b, n_hood2.ID_b, nodes.hop, nodes.distance FROM (SELECT * FROM nodes WHERE (ID_a = ? OR ID_a = ?) AND hop <= ?) n_hood1, (SELECT * FROM nodes WHERE (ID_a = ? OR ID_a = ?) AND hop <= ?) n_hood2,nodes WHERE nodes.ID_a = n_hood1.ID_b AND nodes.ID_b = n_hood2.ID_b AND nodes.hop <= 1 ORDER BY n_hood1.ID_b;";

	int rc = sqlite3_prepare_v2(database, psql, -1, &res, 0);

	char value1[sources.first.size()];
	char value2[sources.second.size()];
	char value4[sources.first.size()];
	char value5[sources.second.size()];

	if (rc == SQLITE_OK)
	{
		strcpy(value1, sources.first.c_str());
		strcpy(value2, sources.second.c_str());
		strcpy(value4, sources.first.c_str());
		strcpy(value5, sources.second.c_str());

		sqlite3_bind_text(res, 1, value1, strlen(value1), 0);
		sqlite3_bind_text(res, 2, value2, strlen(value2), 0);
		sqlite3_bind_int(res, 3, hop);
		sqlite3_bind_text(res, 4, value4, strlen(value4), 0);
		sqlite3_bind_text(res, 5, value5, strlen(value5), 0);
		sqlite3_bind_int(res, 6, hop);
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

vector<vector<ii>> getNHop(vector<vector<ii>> &graph, pair<string, string> sources, intt hop) //,
																							  //    map<intt, intt> &to_indices_nhop, map<intt, intt> &to_node_nhop)
{
	// Add these 3 lines
	vector<intt> intt_sources;
	intt_sources.push_back(to_indices[sources.first]);
	intt_sources.push_back(to_indices[sources.second]);
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
	empty_graph[a].push_back(make_pair(1, b));
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

void callFunctions(pair<string, string> sources, string dest, intt hop, string dataset_name, double output[8])
{
	cout<<"\n"<<omp_get_thread_num()<<"\n";
	to_ind.clear();
	to_no.clear();
	to_indices_nhop.clear();
	to_node_nhop.clear();
	vector<vector<ii>> comb_nbd = getNhop_database(dataset_name, sources, hop); //, to_ind, to_no);
	// vector<vector<ii>> comb_nbd = getNHop(in, sources, hop); //, to_indices_nhop, to_node_nhop); //_without_database
	vector<vector<ii>> comb_nbd_with_edge = addEdge(comb_nbd, to_ind[sources.first], to_ind[sources.second]);
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

	// double output[8];
	double bn_dist_0 = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
	output[0] = bn_dist_0;
	double bn_dist_complete_0 = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
	output[1] = bn_dist_complete_0;
	double w_dist_0 = hera::wasserstein_dist(pdgm, pdgm_with_edge, params, "");
	output[2] = w_dist_0;
	double w_dist_complete_0 = hera::wasserstein_dist(pdgm_complete, pdgm_with_edge, params, "");
	output[3] = w_dist_complete_0;

	pdgm = getDimPD(pd_ab, double(1));
	pdgm_with_edge = getDimPD(pd_ab_with_edge, double(1));
	pdgm_complete = getDimPD(pd_ab_complete, double(1));

	double bn_dist_1 = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
	output[4] = bn_dist_1;
	double bn_dist_complete_1 = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
	output[5] = bn_dist_complete_1;
	double w_dist_1 = hera::wasserstein_dist(pdgm, pdgm_with_edge, params, "");
	output[6] = w_dist_1;
	double w_dist_complete_1 = hera::wasserstein_dist(pdgm_complete, pdgm_with_edge, params, "");
	output[7] = w_dist_complete_1;

	// cout << "\nbn_dist_dim0=" << bn_dist_0 << "\tbn_dist_dim1=" << bn_dist_1;
	// cout << "\tbn_dist_comp_dim0=" << bn_dist_complete_0 << "\tbn_dist_comp_dim1=" << bn_dist_complete_1 << "\n";
	// cout << "\nw_dist_dim0=" << w_dist_0 << "\tw_dist_dim1=" << w_dist_1;
	// cout << "\tw_dist_comp_dim0=" << w_dist_complete_0 << "\tw_dist_comp_dim1=" << w_dist_complete_1 << "\n";
	// return output;
	cout << "\n"
		 << sources.first << "\t"
		 << dest << "\t"
		 << sources.second << ":"
		 << output[0] << ","
		 << output[1] << ","
		 << output[2] << ","
		 << output[3] << ","
		 << output[4] << ","
		 << output[5] << ","
		 << output[6] << ","
		 << output[7] << "\t";
}

int main(int argc, char *argv[])
{
	fastio;

	if (argc < 6)
	{
		cout << "[Usage]: "
			 << "./script dataset_name train_set test_set nbd_hop comb_nbd_hop chunk_size output_file\n";
		return 0;
	}

	string dataset_name = argv[1];
	string train_set = argv[2];
	string test_set = argv[3];
	int nbd_hop = atoi(argv[4]);
	int comb_nbd_hop = atoi(argv[5]);
	int chunk_size = atoi(argv[6]);
	string output_file = argv[7];

	string database_loc = "./database.db";
	char database_loc_proper[database_loc.size()];
	strcpy(database_loc_proper, database_loc.c_str());
	int rc = sqlite3_open(database_loc_proper, &database);

	tsFile.open((string)test_set);
	oFile.open((string)output_file);
	// sFile.open((string) sparse_file, ios::binary);

	intt num_nodes = input(train_set, in, reverse_in, to_indices, to_node); // take input
	if (tsFile)
	{
		int size = 0, left_over_size = 0;
		vector<pair<string, string>> edges;
		vector<string> destinations;
		while (true)
		{
			string source, dest;
			intt nbd_size = 0;
			// cin >> u;
			tsFile >> source >> dest; // read from file

			if (tsFile.eof())
			{
				break; // break it end of file
			}

			else
			{
				vector<string> src_nbd = just_getNhop_database(dataset_name, source, nbd_hop);
				nbd_size = src_nbd.size();
				bool isFoundDest = false;

				if (nbd_size == num_nodes)
				{
					isFoundDest = true;
				}
				else
				{
					for (size_t i = 0; i < nbd_size; i++)
					{
						if (src_nbd[i] == dest)
						{
							isFoundDest = true;
							break;
						}
					}
				}
				if (isFoundDest)
				{
					for (size_t i = 0; i < nbd_size; i++)
					{
						edges.push_back({source, src_nbd[i]});
						destinations.push_back(dest);
					}
					size = size + nbd_size;
				}
				else
				{
					// cout << "\n"
					// 	 << source << "\t" << dest << "\t"
					// 	 << "inf";
					oFile << "\n"
						  << source << "\t" << dest << "\t"
						  << "inf";
				}

				src_nbd.clear();
				// break;
			}
		}
		// cout << "\n Size = " << size << "\n";
		// int a;
		// cin >> a;

		if (size > 0)
		{
			double output[size][8];
				// cout << "\n"
				// 	 << omp_get_num_threads()
				// 	 << "\n"
				// 	 << omp_get_max_threads()
				// 	 << "\n";
			// exit(0);
			////Starting parallel reagion////////////
			intt MAX_THREADS = omp_get_max_threads();
			// int a;
			// cin>>a;
			size_t i;
#pragma omp parallel for shared(edges, destinations, output) private(i)
			for (i = 0; i < size; i++)
			{
				cout << "\n"
					 << omp_get_thread_num() << "\n";

				// callFunctions(edges[i], destinations[i], comb_nbd_hop, dataset_name, output[i]);
			}
			exit(0);
			/////////End of parallel reagion///////////
			for (size_t i = 0; i < size; i++)
			{
				oFile << "\n"
					  << edges[i].first << "\t"
					  << destinations[i] << "\t"
					  << edges[i].second << ":"
					  << output[i][0] << ","
					  << output[i][1] << ","
					  << output[i][2] << ","
					  << output[i][3] << ","
					  << output[i][4] << ","
					  << output[i][5] << ","
					  << output[i][6] << ","
					  << output[i][7] << "\t";

				cout << "\n"
					 << edges[i].first << "\t"
					 << destinations[i] << "\t"
					 << edges[i].second << ":"
					 << output[i][0] << ","
					 << output[i][1] << ","
					 << output[i][2] << ","
					 << output[i][3] << ","
					 << output[i][4] << ","
					 << output[i][5] << ","
					 << output[i][6] << ","
					 << output[i][7] << "\t";

				// cin >> a;
			}
		}
		edges.clear();
		destinations.clear();
	}

	tsFile.close();
	oFile.close();
	sqlite3_close(database);

	return 0;
}
