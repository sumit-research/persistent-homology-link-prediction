#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <set>
#include <queue>
#include <tuple>
#include <map>
#include <limits>
#include <utility>

using namespace std;

#define intt int64_t
#define ii pair<double, intt>

// ifstream iFile;
// ofstream oFile;

vector<intt> bfs(vector<vector<ii>> &in, vector<intt> sources, intt num_nodes)
{
        vector<intt> len(num_nodes + 1, num_nodes + 10); // len[i]-> length of path from source to node i
        vector<bool> visit(num_nodes, 0);       // visited array

        queue<int> q;
        for (int i = 0; i < sources.size(); i++)
        {
                q.push(sources[i]);
                visit[sources[i]] = 1;
                len[sources[i]] = 0;
        }

        while (!q.empty())
        {

                int u = q.front(); // get current node

                for (int i = 0; i < in[u].size(); i++)
                {

                        if (visit[in[u][i].second] == 0)
                        {
                                q.push(in[u][i].second); // push the child node in the queue
                                visit[in[u][i].second] = 1; // mark the node visited
                                len[in[u][i].second] = len[u] + 1; // update length from source to current child node
                        }
                }

                q.pop();
        }
        // cout << "\nlen.size() = " << len.size() << "\n";

        // for (size_t i = 0; i < len.size(); i++)
        // {

        //     if (len[i] > num_nodes)
        //     {
        //         cout << "\n i, len[i] = " << i << " , " << len[i] << '\n';
        //         int a;
        //         cin >> a;
        //     }
        // }

        return len;
}

void write_complete(intt distinct_nodes, int hop, vector<intt> &sources, map<intt, string> &to_node, bool remove, string dataset_name)
{
        string out_path = "/home/deepak/Project/files/outputs/" + dataset_name + "/n_" + to_string(hop) + "/apsp_complete_full_" + to_node[sources[0]] + "_" + to_node[sources[1]];
        if (remove)
        {
                out_path = "/home/deepak/Project/files/outputs/" + dataset_name + "/removed_edge_" + to_string(hop) + "/apsp_complete_full_" + to_node[sources[0]] + "_" + to_node[sources[1]];
        }

        ofstream cFile(out_path);

        for (int i = 0; i < distinct_nodes; i++)
        {
                for (int j = 0; j < i; j++)
                {
                        double distance = (double)(1 - (i == j));
                        cFile << distance;
                        if (j != i - 1)
                                cFile << ',';
                        else
                                cFile << '\n';
                        // cFile.write(reinterpret_cast<const char*>(&distance), sizeof(double));
                }
        }
        cFile.close();
}
