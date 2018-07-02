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
#define ii pair<intt, intt>

ifstream iFile;
ofstream oFile;

vector<intt> bfs(vector<vector< ii > >& in, vector<intt> sources, intt num_nodes){
	
	vector<intt> len(num_nodes,INT64_MAX); // len[i]-> length of path from source to node i
	vector<bool> visit(num_nodes,0); // visited array 

	queue<int> q;
	for(int i = 0; i < sources.size(); i++){
		q.push(sources[i]); 
		visit[sources[i]] = 1;
		len[sources[i]] = 0;
	}

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
				if(find(in[to_indices[dest]].begin(), in[to_indices[dest]].end(), make_pair(weight, to_indices[source])) == in[to_indices[dest]].end())
					in[to_indices[source]].push_back(make_pair(weight, to_indices[source]));
			}	
		}
	}
	// cout << "Edges: " << edges << '\n';
	return i-1; // number of distinct nodes
}

void write_complete(intt distinct_nodes, int hop, vector<intt>& sources, map<intt,intt>& to_node){
	string out_path = "/Users/admin/Desktop/Project/outputs/n_" + to_string(hop) + "/apsp_complete_full_" + to_string(to_node[sources[0]]) + "_" + to_string(to_node[sources[1]]);
	
	ofstream cFile(out_path, ios::binary);

	intt DIPHA = 8067171840,file_type = 7;
	cFile.write(reinterpret_cast<const char*>(&DIPHA), sizeof(intt));
	cFile.write(reinterpret_cast<const char*>(&file_type), sizeof(intt));
	cFile.write(reinterpret_cast<const char*>(&distinct_nodes), sizeof(intt));

	for(int i = 0; i < distinct_nodes; i++){
		for(int j = 0; j < distinct_nodes; j++){
			double distance = (double)(1-(i==j));
			cFile.write(reinterpret_cast<const char*>(&distance), sizeof(double));
		}
	}
	cFile.close();
}

intt write_txt(intt num_nodes, intt hop, vector<intt>& dist, vector<vector< ii > >& in, map<intt,intt> to_node){	
	
	set<intt> s;

	for(intt v = 1; v <= num_nodes; v++){
		if(dist[v] <= hop){
			vector<int> temp;
			for(int i = 0; i < in[v].size(); i++){
				if(dist[in[v][i].second] <= hop)
					temp.push_back(i);
			}

			if(temp.size() > 0){
				oFile << to_node[v] << ":\n";
				s.insert(to_node[v]);
				for(int i = 0; i < temp.size(); i++){
					s.insert(to_node[in[v][temp[i]].second]);
					oFile << to_node[in[v][temp[i]].second] << " " << in[v][temp[i]].first <<'\n';
				}
			}
		}
	}
	return s.size();
}

int main(int argc, char *argv[]){
	string input_file;
	string output_file;
	intt source_1;
	intt source_2;
	intt hop;
	vector<intt> sources;

	if(argc != 5 and argc != 6){
		cout << "[Usage]: " << "./a.out input_filename output_filename source_1 number_of_hops" << '\n';
		cout << "[Usage]: " << "./a.out input_filename output_filename source_1 source_2 number_of_hops" << '\n';
		return 0;
	}
	else if(argc == 5){
		input_file = argv[1];
		output_file = argv[2];
		source_1 = atoi(argv[3]);
		hop = atoi(argv[4]);
	}
	else{
		input_file = argv[1];
		output_file = argv[2];
		source_1 = atoi(argv[3]);
		source_2 = atoi(argv[4]);
		hop = atoi(argv[5]);	
	}

	iFile.open((string) input_file);
	oFile.open((string) output_file);

	map<intt,intt> to_indices; // maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
	map<intt,intt> to_node; // reverse map tp obtain node number using it's index

	vector<vector< ii > > in;

	int num_nodes = input(in, to_indices, to_node); // take input
	// cout << "Input done. "<< num_nodes << "\n";
	
	if(argc == 5){
		sources.push_back(to_indices[source_1]);
	}
	else{
		sources.push_back(to_indices[source_1]);
		sources.push_back(to_indices[source_2]);
	}

// run bfs on the given nodes
	vector<intt> out = bfs(in, sources, num_nodes);

	intt distinct_nodes = write_txt(num_nodes, hop, out, in, to_node);
	if(argc == 6)
		write_complete(distinct_nodes, hop, sources, to_node);
}