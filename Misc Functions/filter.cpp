#include <iostream>
#include <string>
#include <fstream>
#include <set>

using namespace std;

ifstream iFile("smallCitegraph.txt");
ofstream oFile("smallerc.txt");

int main(){

	set<int> distinct;
	long long edges = 0;
	long long max_edge_per_node = 0;
	long long min_edge_per_node = 1000000;
	long long edge_node = 0;
	long long max_vert_index = 0;
	while(true){
		string u;
		// cin >> u;
		iFile >> u;

		if(iFile.eof()) break;

		else{
			if(u[u.length()-1] == ':'){
				string un;
				un = u.substr(0, u.length()-1);
				int u_dec = stoi(un);
				distinct.insert(u_dec);
				max_vert_index = max(max_vert_index, (long long)u_dec);
				max_edge_per_node=max(max_edge_per_node,edge_node);
				if(edge_node != 0)
					min_edge_per_node=min(min_edge_per_node,edge_node);
				edge_node = 0;
				if(u_dec <= 1000)
					oFile << u << '\n';
			}
			else{
				distinct.insert(stoi(u));
				int weight;
				iFile >> weight;
				max_vert_index = max(max_vert_index, (long long)stoi(u));
				edge_node++;
				edges++;
				if(stoi(u) <= 1000)
					oFile << stoi(u) << " " << weight << '\n';
			}
		}
	}
	cout << "Size of Graph:\n";
	cout << "Number of Vertices: " << distinct.size() << '\n';
	cout << "Max vertex number: " << max_vert_index << '\n';
	cout << "Number of Edges: " << edges << '\n';
	cout << "Max edges per node: " << max_edge_per_node << '\n';
	cout << "Min edge per node: " << min_edge_per_node << '\n';
}