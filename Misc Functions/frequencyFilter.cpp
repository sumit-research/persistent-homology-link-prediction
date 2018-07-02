#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include <vector>

using namespace std;

#define intt int64_t

const intt N = 3e7;
ifstream iFile;
ofstream oFile;
ofstream fFile;

intt input(vector<vector<intt> >& adj, vector<vector<intt> >& in, map<intt, intt>& to_indices, map<intt, intt>& to_node){
	string u; 
	intt i = 1;
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
					i++;
				}
			}

			else{
				intt weight;
				dest = stoi(u);

				if(to_indices.find(dest) == to_indices.end()){
					to_indices[dest] = i;
					to_node[i] = dest;
					i++;
				}
				iFile >> weight;
				adj[to_indices[source]].push_back(to_indices[dest]);
				in[to_indices[dest]].push_back(to_indices[source]);
			}	
		}
	}
	return i-1; // number of distinct nodes
}

void calculateFrequency(vector<vector<intt> > vec, map<intt,intt>& frequency, int num_nodes){
	for(int i = 1; i <= num_nodes; i++){
 		frequency[vec[i].size()]++;
	}
}

int write_txt(vector<vector<intt> > adj, vector<vector<intt> > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes, int frequ){
	set<int> newNodes;
	for(int i = 1; i <= num_nodes; i++){

		if((intt)in[i].size() <= frequ){
			// cout << in[i].size() << " " << frequ<< '\n';
			continue;
		}
		else{
			fFile << to_node[i] << ":\n";
			newNodes.insert(to_node[i]);
			for(int j = 0; j < adj[i].size(); j++){
				int node = adj[i][j];
				if(in[node].size() > frequ){
					newNodes.insert(to_node[j]);
					fFile << to_node[node] << " " << 1 <<'\n';
				}
			}
		}
	}
	return newNodes.size();
}

int main(int argc, char* argv[]){

	if(argc < 1){
		cout << "[Usage]: " << "./getFrequency input_filename frequencyFilename filtered_filename filterFrequency\n Program aborted\n";
		return 0;
	}

	string inFile = argv[1];
	// string outFile = argv[2];
	// string filterFile = argv[3];
	// int frequ = (intt)(atoi(argv[4]));
	// cout << "Frequency: " << frequ << '\n';

	iFile.open(inFile);
	// oFile.open(outFile);
	// fFile.open(filterFile);
		
	vector<vector<intt> > adj(N);
	vector<vector<intt> > in(N);
	map<intt,intt> frequency;
	map<intt, intt> to_indices;
	map<intt, intt> to_node;

	intt num_nodes = input(adj, in, to_indices, to_node);

	cout << "Number of nodes in original graph: " << num_nodes << '\n';

	// calculateFrequency(in, frequency, num_nodes);
	int temp = 0;
	int ind = 0;
	vector<pair<int,int> > sizes;
	for(int i = 0; i < N; i++){
		sizes.push_back(make_pair(adj[i].size(), i));
	}
	sort(sizes.rbegin(), sizes.rend());

	for(int i = 0; i < 10; i++){
		cout << sizes[i].first << ' ' << to_node[sizes[i].second] << '\n';
	}
	// oFile << "inDegree,number of nodes\n";
	// for(map<intt, intt>::iterator it = frequency.begin(); it != frequency.end(); it++){
	// 	oFile << it->first << "," << it->second << '\n';
	// }

	// remove nodes with indegree less than 3(to reduce data)
	// int newNodes = write_txt(adj, in, to_indices, to_node, num_nodes, frequ);
	// cout << "Number of nodes in filtered graph: " << newNodes << '\n';
}