#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <map>
#include <limits>
#include <queue>
#include "johnson.cpp"
#include "n_hop_multi.cpp"
#include "ripser.cpp"

using namespace std;
#define intt int64_t
#define ii pair<intt,intt> 
#define fastio	ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0)

ifstream iFile;
ifstream tsfile;
ofstream oFile;
ofstream sFile;
ofstream dFile;

vector<vector<ii>> in;		   // for original graph, to get path from a->b
vector<vector<ii>> reverse_in; // for reverse graph, to get path from b->a

vector<intt> bfs(vector<vector<ii>> &in, vector<intt> sources, intt num_nodes)
{

	vector<intt> len(num_nodes + 1, INT64_MAX); // len[i]-> length of path from source to node i
	vector<bool> visit(num_nodes, 0);			// visited array

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
				q.push(in[u][i].second);		   // push the child node in the queue
				visit[in[u][i].second] = 1;		   // mark the node visited
				len[in[u][i].second] = len[u] + 1; // update length from source to current child node
			}
		}

		q.pop();
	}

	return len;
}

vector<vector<ii>> getNHop(vector<vector<ii>>& empty_g, vector<intt> sources, intt hop)
{
	// vector<intt> sources = {to_indices[a], to_indices[b]};
	map<string, intt> to_indices;  
	map<intt, string> to_node;

	auto dist = bfs(empty_g, sources, empty_graph.size());

	vector<intt> nbd;
	vector<vector<ii>> new_graph;
	current_node = 1;

	new_graph.resize(1);

	for(intt v = 1; v < empty_g.size(); v++){
		if(dist[v] <= hop){

			if(to_indices.find(v) == to_indices.end()){
					to_indices[v] = i;
					to_node[i] = v;
					vector< ii > tmp;
					new_graph.push_back(tmp);
					current_node++;
			}

			source = to_indices[v];

			for(int i = 0; i < empty_g[v].size(); i++){
				if(dist[new_graph[v][i].second] <= hop){
					new_graph[source].push_back(make_pair(empty_g[source][i].first, to_indices[empty_g[source][i].second]));
				}
			}
		}
	}

  return new_graph;
}

vector<vector<ii>> addEdge(string a, string b)
{
	vector<vector<ii>> empty_graph = in;
	empty_graph[a].insert(make_pair(1, b));
	return empty_graph;
}

void callFunctions(intt source, intt dest, int hop)
{

}

int main(int argc, char *argv[]){
	fastio;

	map<string, intt> to_indices;  // maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
	map<intt, string> to_node;	 // reverse map to obtain node number using it's index

	// for taking command line arguements
	int j = 1;

	// used to detect user option for dumping all the pairs
	bool dumping = false;

	if(argc < 6){
		cout << "[Usage]: " << "./script dataset_name train_set test_set nbd_hop output_file\n";
		cout << "Options: \n";
		return 0;
	}

	else if(argc == 7 and (string)argv[1] == "--dump_pairs"){
		j++;
		dumping = true;
	}

	string dataset_name = argv[j++];
	string train_set = argv[j++];
	string test_set = argv[j++];
	int nbd_hop = atoi(argv[j++]);
	string output_file = argv[j++];

	if(dumping)
		dFile.open("./Project/files/outputs/"+dataset_name+"/dumped.txt");

	iFile.open((string) train_set);
	tsfile.open((string) test_set);
	oFile.open((string) output_file);
	// sFile.open((string) sparse_file, ios::binary);

	intt num_nodes = input(to_indices, to_node); // take input

	if(num_nodes == 1){
		oFile << (double) 0 << '\n';
		iFile.close();
		tsfile.close();
		oFile.close();
		sFile.close();
		dFile.close();
		return 0;
	}

	/*
if(os.path.isfile(edges_out)):
		t = open(edges_out, "r")
		random_edges = t.readlines()
		random_edges = [e.strip().split() for e in random_edges]
		t.close()

	else:
	# get 100 edges at random
		random_edges = get_random_edges(data, sample_size)
		t = open(edges_out, "w")
		for line in random_edges:
			t.write(str(line[0] + " " + str(line[1] + '\n')))
		t.close()

*/

	if (tsfile != NULL)
	{
		while (true)
		{
			string source, dest;
			// cin >> u;
			tsfile >> source >> dest; // read from file

			if (tsfile.eof())
			{
				break; // break it end of file
			}

			else
			{

				
			}
		}
		tsfile.close();
	}
		
		iFile.close();
		oFile.close();
		sFile.close();
		dFile.close();

		return 0;
}