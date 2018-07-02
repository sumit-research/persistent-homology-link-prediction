#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <map>
#include <limits>
#include <utility>

using namespace std;

#define intt int64_t
#define ii pair<intt, intt>
ifstream iFile;
ofstream oFile;
ofstream sFile;
// sFile.imbue(locale::classic());

vector<intt> bfs(vector<vector< ii > >& in, intt source, intt num_nodes){
	
	vector<intt> len(num_nodes,INT64_MAX); // len[i]-> length of path from source to node i
	vector<bool> visit(num_nodes,0); // visited array 
	len[source] = 0;

	queue<int> q;
	q.push(source); 
	visit[source] = 1;

	while(!q.empty()){

		int u = q.front(); // get current node

		for(int i = 0; i < in[u].size(); i++){		
			
			if(visit[in[u][i].second] == 0){
				q.push(in[u][i].second); // push the child node in the queue
				visit[in[u][i].second] = 1; // mark the node visited
				len[in[u][i].second] = len[u]+1; // update length from source to current child node
			}

		}

		q.pop();
	}

	return len;
}

intt input(vector<vector< ii > >& in, map<intt,intt>& to_indices, map<intt,intt>& to_node){
	string u; 
	intt edges = 0;
	int num_sources = 0;
	intt i = 1;
	in.resize(1);
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
					vector< ii > tmp;
					in.push_back(tmp);
					i++;
				}

			}

			else{
				edges++;
				intt weight;
				dest = stoi(u);

				if(to_indices.find(dest) == to_indices.end()){
					to_indices[dest] = i;
					to_node[i] = dest;
					vector< ii > tmp;
					in.push_back(tmp);
					i++;
				}
				iFile >> weight;
				in[to_indices[source]].push_back(make_pair(weight, to_indices[dest])); 
			}	
		}
	}
	// cout << "Edges: " << edges << '\n';
	return i-1; // number of distinct nodes
}

void write_txt(intt src, intt num_nodes, intt hop, vector<intt>& dist, vector<vector< ii > >& in, map<intt,intt> to_node){		
	for(intt v = 1; v <= num_nodes; v++){
		if(dist[v] <= hop){
			vector<int> temp;
			for(int i = 0; i < in[v].size(); i++){
				if(dist[in[v][i].second] <= hop)
					temp.push_back(i);
			}

			if(temp.size() > 0){
				oFile << to_node[v] << ":\n";
				for(int i = 0; i < temp.size(); i++)
					oFile << to_node[in[v][temp[i]].second] << " " << in[v][temp[i]].first <<'\n';
			}
		}
	}
}

int main(int argc, char *argv[]){

	if(argc < 5){
		cout << "[Usage]: " << "./a.out input_filename output_filename source number_of_hops" << '\n';
		return 0;
	}

	string input_file = argv[1];
	string output_file = argv[2];
	intt source = atoi(argv[3]);
	intt hop = atoi(argv[4]);

	iFile.open((string) input_file);
	oFile.open((string) output_file);

	map<intt,intt> to_indices; // maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
	map<intt,intt> to_node; // reverse map tp obtain node number using it's index

	vector<vector< ii > > in;

	int num_nodes = input(in, to_indices, to_node); // take input
	// cout << "Input done. "<< num_nodes << "\n";
	source = to_indices[source];

	// for(int i = 0; i < num_nodes; i++){
	// 	cout << to_node[i] << ":\n";
	// 	for(int j = 0; j < in[i].size(); j++){
	// 		cout << to_node[in[i][j]] << '\n';
	// 	}
	// }

	// to get sparse matrix representation

	// run bfs for every node

	vector<intt> out = bfs(in, source, num_nodes);

	write_txt(source, num_nodes, hop, out, in, to_node);
}