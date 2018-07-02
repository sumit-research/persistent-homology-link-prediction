#include "includes.h"

intt input(string inputFilename, vector<vector<intt> >& adj, vector<vector<intt> >& in, map<intt, intt>& to_indices, map<intt, intt>& to_node){
	ifstream iFile(inputFilename);
	string u; 
	intt i = 1;
	while(true){
		intt source, dest;
		string u;
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
				adj[to_indices[source]].push_back(to_indices[dest]);
				in[to_indices[dest]].push_back(to_indices[source]);
			}	
		}
	}
	return i-1; // number of distinct nodes
}

intt input(string inputFilename, vector<vector< ii > >& adj, vector<vector< ii > >& in, map<intt, intt>& to_indices, map<intt, intt>& to_node){
	ifstream iFile(inputFilename);
	string u; 
	intt i = 1;
	while(true){
		intt source, dest;
		string u;
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
				iFile >> weight;
				dest = stoi(u);

				if(to_indices.find(dest) == to_indices.end()){
					to_indices[dest] = i;
					to_node[i] = dest;
					i++;
				}
				adj[to_indices[source]].push_back(make_pair(weight, to_indices[dest]));
				in[to_indices[dest]].push_back(make_pair(weight, to_indices[source]));
			}	
		}
	}
	return i-1; // number of distinct nodes
}

void calculateFrequency(vector<vector< intt > > vec, map<intt,intt>& frequency, int num_nodes){
	for(int i = 1; i <= num_nodes; i++){
		frequency[vec[i].size()]++;
	}
}

void calculateFrequency(vector<vector< ii > > vec, map<intt,intt>& frequency, int num_nodes){
	for(int i = 1; i <= num_nodes; i++){
		frequency[vec[i].size()]++;
	}
}

int write_txt(string filename, vector<vector<intt> > adj, vector<vector<intt> > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes, int frequ){
	ofstream fFile(filename);
	set<int> newNodes;
	for(int i = 1; i <= num_nodes; i++){
		if(in[i].size() <= frequ){
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

int write_txt(string filename, vector<vector< ii > > adj, vector<vector< ii > > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes, int frequ){
	ofstream fFile(filename);
	set<int> newNodes;
	for(int i = 1; i <= num_nodes; i++){
		if(in[i].size() <= frequ){
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

int write_txt(string filename, vector<vector<intt> > adj, vector<vector<intt> > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes){
	ofstream fFile(filename);	
	set<int> newNodes;
	for(int i = 1; i <= num_nodes; i++){
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
	return newNodes.size();
}

int write_txt(string filename, vector<vector< ii > > adj, vector<vector< ii > > in, map<intt,intt> to_indices, map<intt,intt> to_node, int num_nodes){
	ofstream fFile(filename);	
	set<int> newNodes;
	for(int i = 1; i <= num_nodes; i++){
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
	return newNodes.size();
}

void write_sparse(string filename, vector<intt> connections, vector<intt> indices, vector<intt> weight, intt num_nodes){
	ofstream sFile(filename);
	
	intt DIPHA = 8067171840,file_type = 8;
	sFile << DIPHA << '\n';
	sFile << file_type << '\n';
	sFile << num_nodes << '\n';
	for(int i = 0; i < connections.size(); i++){
		sFile << connections[i];
		if(i != connections.size())
			sFile << " ";
		else
			sFile << '\n';
	}
	for(int i = 0; i < indices.size(); i++){
		sFile << indices[i];
		if(i != indices.size())
			sFile << " ";
		else
			sFile << '\n';
	}
	for(int i = 0; i < weight.size(); i++){
		sFile << weight[i];
		if(i != weight.size())
			sFile << " ";
		else
			sFile << '\n';
	}
}

#endif