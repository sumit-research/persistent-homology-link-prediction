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
// #include "./bottleneck/bottleneck.h"s
#include "./grey_narn_hera/geom_bottleneck/include/bottleneck.h"
// #include "./wasserstein/wasserstein.h"
#include "./grey_narn_hera/geom_matching/wasserstein/include/wasserstein.h"
using namespace std;
vector<vector<ii>> in;
vector<vector<ii>> reverse_in;
unordered_map<string, intt> to_ind;
unordered_map<intt, string> to_no;
unordered_map<intt, intt> to_indices_nhop;
unordered_map<intt, intt> to_node_nhop;

unordered_map<string, intt> to_indices; // maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
unordered_map<intt, string> to_node;    // reverse map to obtain node number using it's index

// SQL parameters
sqlite3 *database;
sqlite3_stmt *res;
char *err_msg = 0;

ifstream tsFile;
ofstream oFile;
vector<string> just_getNhop_database(string dataset_name, string source, intt hop)
{

        char *psql = "SELECT DISTINCT ID_b FROM nodes WHERE ID_a = ? AND hop <= ? and hop != 0;";

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
        //  cout << nhood_list[i] << '\n';
        // }
        return nhood_list;
}

vector<vector<ii>> getNhop_database(string dataset_name, string sources[2], intt hop)
{

        char *psql = "WITH nhood1 as (SELECT ID_b FROM nodes WHERE ID_a = ? and hop <= ? UNION ALL Select ID_b FROM nodes where ID_a = ? AND hop <= ?) Select * FROM nodes Where ID_a IN nhood1 AND ID_b IN nhood1 AND nodes.HOP=1;";

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

                if (to_ind.find(temp[3]) == to_ind.end())
                {
                        to_ind[temp[3]] = current_node;
                        to_no[current_node] = temp[3];
                        vector<ii> tmp;
                        nhood_graph.push_back(tmp);
                        current_node++;
                }
                if (to_ind.find(temp[4]) == to_ind.end())
                {
                        to_ind[temp[4]] = current_node;
                        to_no[current_node] = temp[4];
                        vector<ii> tmp;
                        nhood_graph.push_back(tmp);
                        current_node++;
                }

                nhood_graph[to_ind[temp[3]]].push_back(make_pair(stod(temp[1]), to_ind[temp[4]]));
                // results.push_back(temp);

                rc = sqlite3_step(res);
        }
        if (nhood_graph.size() == 0)
        {
                vector<ii> tmp;
                nhood_graph.push_back(tmp);
                nhood_graph.push_back(tmp);
                sqlite3_finalize(res);
                return nhood_graph;
        }

        sqlite3_finalize(res);
        //   sqlite3_close(database);

        // cout << "With SQL\n";
        // for (int i = 1; i < nhood_graph.size(); i++)
        // {
        //  cout << to_no[i] << "-> ";
        //  for (int j = 0; j < nhood_graph[i].size(); j++)
        //      cout << to_no[nhood_graph[i][j].second] << " ";

        //  cout << '\n';
        // }

        // remove the edge between two sources if it exists

        // // find the position
        // cout << "here171\n";
        intt p = to_ind[sources[1]];
        // cout << "here173\n";
        vector<ii>::iterator it;
        for(int i = 0; i < nhood_graph[to_ind[sources[0]]].size(); i++) {
                if(nhood_graph[to_ind[sources[0]]][i].second == p) {
                        it = nhood_graph[to_ind[sources[0]]].begin()+i;
                        nhood_graph[to_ind[sources[0]]].erase(it);
                        break;
                }
        }

        p = to_ind[sources[0]];
        for(int i = 0; i < nhood_graph[to_ind[sources[1]]].size(); i++) {
                if(nhood_graph[to_ind[sources[1]]][i].second == p) {
                        it = nhood_graph[to_ind[sources[1]]].begin()+i;
                        nhood_graph[to_ind[sources[1]]].erase(it);
                        break;
                }
        }

        return nhood_graph;
}

vector<vector<ii>> getNhop_database(string dataset_name, string source, intt hop)
{

        char *psql = "WITH nhood1 as (SELECT ID_b FROM nodes WHERE ID_a = ? and hop <= ?) Select * FROM nodes Where ID_a IN nhood1 AND ID_b IN nhood1 AND nodes.HOP=1;";

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

                if (to_ind.find(temp[3]) == to_ind.end())
                {
                        to_ind[temp[3]] = current_node;
                        to_no[current_node] = temp[3];
                        vector<ii> tmp;
                        nhood_graph.push_back(tmp);
                        current_node++;
                }
                if (to_ind.find(temp[4]) == to_ind.end())
                {
                        to_ind[temp[4]] = current_node;
                        to_no[current_node] = temp[4];
                        vector<ii> tmp;
                        nhood_graph.push_back(tmp);
                        current_node++;
                }

                nhood_graph[to_ind[temp[3]]].push_back(make_pair(stod(temp[1]), to_ind[temp[4]]));
                // results.push_back(temp);

                rc = sqlite3_step(res);
        }
        if (nhood_graph.size() == 1)
        {
                vector<ii> tmp;
                nhood_graph.push_back(tmp);
        }

        sqlite3_finalize(res);
        //   sqlite3_close(database);

        // cout << "With SQL single neighborhood\n";
        // for (int i = 1; i < nhood_graph.size(); i++)
        // {
        //  cout << to_no[i] << "-> ";
        //  for (int j = 0; j < nhood_graph[i].size(); j++)
        //      cout << to_no[nhood_graph[i][j].second] << " ";

        //  cout << '\n';
        // }
        return nhood_graph;
}

vector<vector<ii>> getNHop(vector<vector<ii>> &graph, vector<string> sources, intt hop)
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

        // cout << "\nWithout SQL\n";
        // for (int i = 1; i < new_graph.size(); i++)
        // {
        //  cout << to_node[to_node_nhop[i]] << "-> ";
        //  for (int j = 0; j < new_graph[i].size(); j++)
        //      cout << to_node[to_node_nhop[new_graph[i][j].second]] << " ";

        //  cout << '\n';
        // }
        return new_graph;
}

vector<vector<ii>> addEdge(vector<vector<ii>> graph, intt a, intt b, bool directed)
{
        vector<vector<ii>> empty_graph = graph;
        empty_graph[a].push_back(make_pair(1.0, b));

        // ask sir
        if(!directed){
            empty_graph[b].push_back(make_pair(1.0, a));
        }
        return empty_graph;
}

vector<vector<double>> getPD(intt graph_size, double threshold)
{
        vector<vector<double>> outvec(graph_size);

        for (size_t i = 0; i < graph_size - 1; i++)
        {
                outvec[i].push_back(0);
                outvec[i].push_back(0);
                outvec[i].push_back(1);
        }

        outvec[graph_size - 1].push_back(0);
        outvec[graph_size - 1].push_back(0);
        outvec[graph_size - 1].push_back(threshold);
        return outvec;
        // vector<vector<double>> apsp;
        // for (intt i = 0; i < graph_size; i++)
        // {
        //     apsp.push_back(vector<double>(graph_size + 1));
        //     for (intt j = 1; j <= graph_size; j++)
        //     {
        //         apsp[i][j] = 1.0;
        //     }
        // }
        // return call_ripser(1, threshold, apsp); //threshold = 4 is for some reason. max dimension = 1 required by us.
}

vector<vector<double>> getPD(vector<vector<ii>> small_graph, double threshold)
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
        return call_ripser(1, threshold, apsp); //threshold = 4 is for some reason. max dimension = 1 required by us.
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

void callFunctions(string sources[2], intt hop, string dataset_name, double output[8], double threshold)
{
        to_ind.clear();
        to_no.clear();
        // to_indices_nhop.clear();
        // to_node_nhop.clear();
        vector<vector<ii>> comb_nbd = getNhop_database(dataset_name, sources, hop); //, to_ind, to_no);

        // vector<vector<ii>> comb_nbd_waste = getNHop(in, sources, hop); //, to_indices_nhop, to_node_nhop); //_without_database
        vector<vector<ii>> comb_nbd_with_edge = addEdge(comb_nbd, to_ind[sources[0]], to_ind[sources[1]]);
        // To get neighborhood of single node

        // vector<vector<ii>> comb_nbd_with_edge = addEdge(comb_nbd, to_indices_nhop[to_indices[sources[0]]], to_indices_nhop[to_indices[sources[1]]]);
        to_ind.clear();
        to_no.clear();
        vector<vector<ii>> a_nbd = getNhop_database(dataset_name, sources[0], hop);
        vector<vector<double>> pd_a = getPD(a_nbd, threshold);
        to_ind.clear();
        to_no.clear();
        vector<vector<ii>> b_nbd = getNhop_database(dataset_name, sources[1], hop);
        vector<vector<double>> pd_b = getPD(b_nbd, threshold);

        vector<vector<double>> pd_ab = getPD(comb_nbd, threshold);
        vector<vector<double>> pd_ab_with_edge = getPD(comb_nbd_with_edge, threshold);
        vector<vector<double>> pd_ab_complete = getPD(comb_nbd.size() - 1, threshold);
        vector<pair<double, double>> pdgm_a = getDimPD(pd_a, double(0));
        vector<pair<double, double>> pdgm_b = getDimPD(pd_b, double(0));
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

        double w_dist = hera::wasserstein_dist(pdgm, pdgm_with_edge, params, "");
        output[0] = w_dist;
        double w_dist_complete = hera::wasserstein_dist(pdgm_complete, pdgm_with_edge, params, "");
        output[1] = w_dist_complete;
        double w_dist_a = hera::wasserstein_dist(pdgm_a, pdgm_with_edge, params, "");
        output[2] = w_dist_a;
        double w_dist_b = hera::wasserstein_dist(pdgm_b, pdgm_with_edge, params, "");
        output[3] = w_dist_b;

        double bn_dist = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
        output[4] = bn_dist;
        double bn_dist_complete = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
        output[5] = bn_dist_complete;
        double bn_dist_a = hera::bottleneckDistExact(pdgm_a, pdgm_with_edge);
        output[6] = bn_dist_a;
        double bn_dist_b = hera::bottleneckDistExact(pdgm_b, pdgm_with_edge);
        output[7] = bn_dist_b;

        // pdgm.clear();
        // pdgm_a.clear();
        // pdgm_b.clear();
        // pdgm_with_edge.clear();
        // pdgm_complete.clear();
        // pdgm_a = getDimPD(pd_a, double(1));
        // pdgm_b = getDimPD(pd_b, double(1));
        // pdgm = getDimPD(pd_ab, double(1));
        // pdgm_with_edge = getDimPD(pd_ab_with_edge, double(1));
        // pdgm_complete = getDimPD(pd_ab_complete, double(1));

        // w_dist = hera::wasserstein_dist(pdgm, pdgm_with_edge, params, "");
        // output[8] = w_dist;
        // w_dist_complete = hera::wasserstein_dist(pdgm_complete, pdgm_with_edge, params, "");
        // output[9] = w_dist_complete;
        // w_dist_a = hera::wasserstein_dist(pdgm_a, pdgm_with_edge, params, "");
        // output[10] = w_dist_a;
        // w_dist_b = hera::wasserstein_dist(pdgm_b, pdgm_with_edge, params, "");
        // output[11] = w_dist_b;

        // bn_dist = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
        // output[12] = bn_dist;
        // bn_dist_complete = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
        // output[13] = bn_dist_complete;
        // bn_dist_a = hera::bottleneckDistExact(pdgm_a, pdgm_with_edge);
        // output[14] = bn_dist_a;
        // bn_dist_b = hera::bottleneckDistExact(pdgm_b, pdgm_with_edge);
        // output[15] = bn_dist_b;
}

void callFunctions(string sources[2], intt hop, string dataset_name, vector<vector<double>> pd_a, double output[1], double threshold)
{
        // vector<double> output;
        to_ind.clear();
        to_no.clear();
        // to_indices_nhop.clear();
        // to_node_nhop.clear();
        vector<vector<ii>> comb_nbd = getNhop_database(dataset_name, sources, hop); //, to_ind, to_no);
        // cout << "\nsources:(" << sources[0] << "," << sources[1] << ") comb_nbd.size() = " << comb_nbd.size() << "\n";
        // for (size_t i = 0; i < comb_nbd.size(); i++)
        // {
        //     for (size_t j = 0; j < comb_nbd[i].size(); j++)
        //     {
        //         cout << "\nb_nbd[i][j]\t"<<i<<","<<j<<":"
        //              << comb_nbd[i][j].first << "," << comb_nbd[i][j].second << "\n";
        //     }
        // }

        // vector<vector<ii>> comb_nbd_waste = getNHop(in, sources, hop); //, to_indices_nhop, to_node_nhop); //_without_database
        vector<vector<ii>> comb_nbd_with_edge = addEdge(comb_nbd, to_ind[sources[0]], to_ind[sources[1]]);
        // To get neighborhood of single node

        // vector<vector<ii>> comb_nbd_with_edge = addEdge(comb_nbd, to_indices_nhop[to_indices[sources[0]]], to_indices_nhop[to_indices[sources[1]]]);
        to_ind.clear();
        to_no.clear();
        vector<vector<ii>> b_nbd = getNhop_database(dataset_name, sources[1], hop);
        // cout << "\nsources:(" << sources[0] << "," << sources[1] << ") b_nbd.size() = " << b_nbd.size() << "\n";
        // for (size_t i = 0; i < b_nbd.size(); i++)
        // {
        //     for (size_t j = 0; j < b_nbd[i].size(); j++)
        //     {
        //         cout << "\nb_nbd[i][j]\t" << i << "," << j << ":"
        //              << b_nbd[i][j].first << "," << b_nbd[i][j].second << "\n";
        //     }
        // }
        // int a;
        // cin >> a;

        vector<vector<double>> pd_b = getPD(b_nbd, threshold);

        // if (b_nbd.size() == 2)
        // {
        //     cout << "\npd_b.size()="
        //          << pd_b.size() << "\n";
        //     for (size_t i = 0; i < b_nbd.size(); i++)
        //     {
        //         for (size_t j = 0; j < b_nbd[i].size(); j++)
        //         {
        //             cout << "\nb_nbd[i][j]\n"
        //                  << b_nbd[i][j].first << "," << b_nbd[i][j].second << "\n";
        //         }
        //     }

        //     for (size_t i = 0; i < pd_b.size(); i++)
        //     {
        //         cout << "\n"
        //              << pd_b[i][0] << ","
        //              << pd_b[i][1] << ","
        //              << pd_b[i][2] << "\n";
        //     }
        //     int a;
        //     cin >> a;
        // }

        // cout << "\n476\n";
        vector<vector<double>> pd_ab = getPD(comb_nbd, threshold);
        // cout << "\n478\n";
        vector<vector<double>> pd_ab_with_edge = getPD(comb_nbd_with_edge, threshold);
        // cout << "\n480\n";
        vector<vector<double>> pd_ab_complete = getPD(comb_nbd.size() - 1, threshold);
        // cout << "\n482\n";
        vector<pair<double, double>> pdgm_a = getDimPD(pd_a, double(0));
        vector<pair<double, double>> pdgm_b = getDimPD(pd_b, double(0));
        vector<pair<double, double>> pdgm = getDimPD(pd_ab, double(0));
        vector<pair<double, double>> pdgm_with_edge = getDimPD(pd_ab_with_edge, double(0));
        vector<pair<double, double>> pdgm_complete = getDimPD(pd_ab_complete, double(0));
        // for (size_t i = 0; i < pdgm_complete.size(); i++)
        // {
        //     cout << "\n0-D\t" << pdgm_complete.size() << "," << comb_nbd.size() << "," << pd_ab_complete.size() << ","
        //          << pdgm_complete[i].first << "," << pdgm_complete[i].second << "\n";
        // }

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

        // double bn_dist_0 = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
        // output.push_back(bn_dist_0);
        // double bn_dist_complete_0 = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
        // output.push_back(bn_dist_complete_0);
        double w_dist = hera::wasserstein_dist(pdgm, pdgm_with_edge, params, "");
        output[0] = w_dist;
        // cout << "\nw_dist0\t" << w_dist << "\n";
        double w_dist_complete = hera::wasserstein_dist(pdgm_complete, pdgm_with_edge, params, "");
        output[1] = w_dist_complete;
        // cout << "\nw_dist_complete0\t" << w_dist_complete << "\n";
        double w_dist_a = hera::wasserstein_dist(pdgm_a, pdgm_with_edge, params, "");
        output[2] = w_dist_a;
        // cout << "\nw_dist_a0\t" << w_dist_a << "\n";
        double w_dist_b = hera::wasserstein_dist(pdgm_b, pdgm_with_edge, params, "");
        output[3] = w_dist_b;
        // cout << "\nw_dist_b0\t" << w_dist_b << "\t" << pdgm_b.size() << "\n";

        double bn_dist = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
        output[4] = bn_dist;
        // cout << "\nbn_dist0\t" << bn_dist << "\n";
        double bn_dist_complete = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
        output[5] = bn_dist_complete;
        // cout << "\nbn_dist_complete0\t" << bn_dist_complete << "\n";
        double bn_dist_a = hera::bottleneckDistExact(pdgm_a, pdgm_with_edge);
        output[6] = bn_dist_a;
        // cout << "\nbn_dist_a0\t" << bn_dist_a << "\t" << pdgm_b.size() << "\n";

        // if (pdgm_b.size() == 1)
        // {
        //     cout << "\nb_nbd.size()\t" << b_nbd.size() << "\n";
        //     for (size_t i = 0; i < b_nbd.size(); i++)
        //     {

        //         for (size_t j = 0; j < b_nbd[i].size(); j++)
        //         {
        //             cout << "\nb_nbd[i][j]\n"
        //                  << b_nbd[i][j].first << "," << b_nbd[i][j].second << "\n";
        //         }
        //     }
        //     for (size_t i = 0; i < pdgm_b.size(); i++)
        //     {
        //         cout << "\n"
        //              << pdgm_b[i].first << "," << pdgm_b[i].second << "\n";
        //     }

        //     // pdgm_b[0].second = 4;
        // }
        double bn_dist_b = hera::bottleneckDistExact(pdgm_b, pdgm_with_edge);
        output[7] = bn_dist_b;
        // cout << "\nbn_dist_b0\t" << bn_dist_b << "\n";

        // pdgm.clear();
        // pdgm_a.clear();
        // pdgm_b.clear();
        // pdgm_with_edge.clear();
        // pdgm_complete.clear();
        // pdgm_a = getDimPD(pd_a, double(1));
        // pdgm_b = getDimPD(pd_b, double(1));
        // pdgm = getDimPD(pd_ab, double(1));
        // pdgm_with_edge = getDimPD(pd_ab_with_edge, double(1));
        // pdgm_complete = getDimPD(pd_ab_complete, double(1));
        // // for (size_t i = 0; i < pdgm_complete.size(); i++)
        // // {
        // //     cout << "\n1-D\t" << pdgm_complete.size() << ","
        // //          << pdgm_complete[i].first << "," << pdgm_complete[i].second << "\n";
        // // }

        // w_dist = hera::wasserstein_dist(pdgm, pdgm_with_edge, params, "");
        // output[8] = w_dist;
        // // cout << "\nw_dist1\t" << w_dist << "\n";
        // w_dist_complete = hera::wasserstein_dist(pdgm_complete, pdgm_with_edge, params, "");
        // output[9] = w_dist_complete;
        // // cout << "\nw_dist_complete1\t" << w_dist_complete << "\n";
        // w_dist_a = hera::wasserstein_dist(pdgm_a, pdgm_with_edge, params, "");
        // output[10] = w_dist_a;
        // // cout << "\nw_dist_a1\t" << w_dist_a << "\n";
        // w_dist_b = hera::wasserstein_dist(pdgm_b, pdgm_with_edge, params, "");
        // output[11] = w_dist_b;
        // // cout << "\nw_dist_b1\t" << w_dist_b << "\n";

        // bn_dist = hera::bottleneckDistExact(pdgm, pdgm_with_edge);
        // output[12] = bn_dist;
        // // cout << "\nbn_dist1\t" << bn_dist << "\n";
        // bn_dist_complete = hera::bottleneckDistExact(pdgm_complete, pdgm_with_edge);
        // output[13] = bn_dist_complete;
        // // cout << "\nbn_dist_complete1\t" << bn_dist_complete << "\n";
        // bn_dist_a = hera::bottleneckDistExact(pdgm_a, pdgm_with_edge);
        // output[14] = bn_dist_a;
        // // cout << "\nbn_dist_a1\t" << bn_dist_a << "\n";
        // bn_dist_b = hera::bottleneckDistExact(pdgm_b, pdgm_with_edge);
        // output[15] = bn_dist_b;
        // // cout << "\nbn_dist_b1\t" << bn_dist_b << "\n";
}
