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

using namespace std;
#define intt int64_t
#define ii pair<intt, intt>

void dijsktra(vector<vector<ii>> &adj, vector<intt> &dist, intt src, intt num_nodes)
{
    dist[src] = 0;
    // cout << "Source: " << src << '\n';
    priority_queue<ii, vector<ii>, greater<ii>> pq; // make priority queue for storing pairs(distance , vertex)
    pq.push(make_pair(0, src));

    while (!pq.empty())
    {
        ii u = pq.top(); // get the minimum weight element
        pq.pop();

        // for every edge u-v, update the distance of v from source
        for (intt i = 0; i < adj[u.second].size(); i++)
        {
            ii v = adj[u.second][i];

            // update condition
            if (dist[v.second] > dist[u.second] + v.first)
            {
                dist[v.second] = dist[u.second] + v.first;
                pq.push(make_pair(dist[v.second], v.second)); // push the new distance, vertex pair in the queue
            }
        }
    }

    return;
}

bool bellman(vector<vector<ii>> &adj, vector<intt> &dist, intt src, intt num_nodes)
{
    dist[src] = 0;

    for (intt i = 1; i < num_nodes - 1; i++)
    {

        // for every edge
        for (intt u = 0; u <= num_nodes; u++)
        {
            for (intt i = 0; i < adj[u].size(); i++)
            {

                // update the distance of node v from source vertex
                ii v = adj[u][i];

                if (dist[v.second] > dist[u] + v.first)
                {
                    dist[v.second] = dist[u] + v.first;
                }
            }
        }
    }

    for (intt u = 0; u <= num_nodes; u++)
    {
        for (intt i = 0; i < adj[u].size(); i++)
        {

            // if you can find a shorter path from source to some vertex then report negative cycle
            ii v = adj[u][i];

            if (dist[v.second] > dist[u] + v.first)
            {
                return false;
            }
        }
    }

    return true;
}

vector<vector<double>> johnson(vector<vector<ii>> &adj, vector<vector<ii>> &reverse_adj, intt num_nodes)
{

    vector<intt> dist(num_nodes + 1, INT64_MAX);
    vector<intt> reverse_dist(num_nodes + 1, INT64_MAX);
    vector<double> distance(num_nodes + 1, 0.0);
    vector<vector<double>> apsp;

    // run djikstra for every vertex
    // cout << "Starting..\n";
    for (intt u = 1; u <= num_nodes; u++)
    {

        //check progress
        // if (u % 1000 == 0)
        // {
        //     cout << "Done till " << u << "th node.\n";
        // }

        fill(dist.begin(), dist.end(), INT64_MAX);
        fill(reverse_dist.begin(), reverse_dist.end(), INT64_MAX);

        dist[u] = reverse_dist[u] = 0;

        if (adj[u].size() != 0)
            dijsktra(adj, dist, u, num_nodes);

        if (reverse_adj[u].size() != 0)
            dijsktra(reverse_adj, reverse_dist, u, num_nodes);

        // get old edge/path weights back
        for (intt v = 1; v <= num_nodes; v++)
        {
            if (dist[v] == INT64_MAX)
                dist[v] = 100;
            if (reverse_dist[v] == INT64_MAX)
                reverse_dist[v] = 100;

            distance[v] = ((double)dist[v] + (double)reverse_dist[v]) / 2.0;
            // if(dist[v] == 5 or dist[v] == 6 or dist[v] == 3)
            // 	cout << dist[v] << " dest-> " << to_node[v] << " sourcce-> " << to_node[u] << '\n';
        }
        apsp.push_back(distance);
    }
    return apsp;
}

intt input(string train_set, vector<vector<ii>> &in, vector<vector<ii>> &reverse_in,
           unordered_map<string, intt> &to_indices, unordered_map<intt, string> &to_node)
{
    ifstream iFile;
    iFile.open(train_set);
    string u;
    intt edges = 0;
    int num_sources = 0;
    intt i = 1;
    in.resize(1);
    reverse_in.resize(1);
    string source, dest;
    while (true)
    {
        string u;
        // cin >> u;
        iFile >> u; // read from file

        if (iFile.eof())
        {
            break; // break it end of file
        }

        else
        {

            // check for the source node
            if (u[u.length() - 1] == ':')
            {
                string un;
                source = u.substr(0, u.length() - 1); // remove colon

                if (to_indices.find(source) == to_indices.end())
                {
                    to_indices[source] = i;
                    to_node[i] = source;
                    vector<ii> tmp;
                    in.push_back(tmp);
                    reverse_in.push_back(tmp);
                    i++;
                }
            }

            else
            {
                edges++;
                intt weight;
                dest = u;

                if (to_indices.find(dest) == to_indices.end())
                {
                    to_indices[dest] = i;
                    to_node[i] = dest;
                    vector<ii> tmp;
                    in.push_back(tmp);
                    reverse_in.push_back(tmp);
                    i++;
                }
                iFile >> weight;
                // cout << source << " " << dest << "\n";
                in[to_indices[source]].push_back(make_pair(weight, to_indices[dest]));
                reverse_in[to_indices[dest]].push_back(make_pair(weight, to_indices[source]));

                // add reverse edge if it doens't already exist
                // if(find(in[to_indices[dest]].begin(), in[to_indices[dest]].end(), make_pair(weight, to_indices[source])) == in[to_indices[dest]].end())
                // 	in[to_indices[dest]].push_back(make_pair(weight, to_indices[source]));
            }
        }
    }
    // cout << "Edges: " << edges << '\n';
    return i - 1; // number of distinct nodes
}
