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

using namespace std;
#define intt int64_t
#define ii pair<intt,intt> 
#define fastio	ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0)


ifstream iFile;
ofstream oFile;
ofstream sFile;
ofstream dFile;
// ofstream indFile("indices.txt");
// ofstream wFile("weight.txt");
// ofstream cFile("connections.txt");


// sFile.imbue(locale::classic());

vector< vector< ii > > in; // for original graph, to get path from a->b
vector< vector< ii > > reverse_in; // for reverse graph, to get path from b->a
map<string,intt> to_indices; // maps node to index(1-n), index is always from 1-n where n is number of distinct nodes
map<intt,string> to_node; // reverse map tp obtain node number using it's index

void write_txt(intt src, vector<double>& dist, intt num_nodes){

	intt temp = 0; // to calcualte number of connections

	for(intt v = 1; v < src; v++){

		double distance = static_cast<double>(dist[v]);
		// if(distance == 1){
		// 	cout << 1 << " dest-> " << to_node[v] << " source-> " << to_node[src] << '\n';
		// }
		if(distance== 0 and v != src){
			cout << src << " " << v << '\n';
		}

		oFile << distance;
		if(v != src-1)
			oFile << ',';
		else
			oFile << '\n';

		// oFile.write(reinterpret_cast<const char*>(&distance), sizeof(double));

		// if(dist[v] <= num_nodes and v != src){
		// 	temp++;
		// 	indFile << (v-1) << '\n';
		// 	wFile << dist[v] << '\n'; 
		// }
	}
	// cFile << temp << '\n' << flush;
}

void write_sparse(intt num_nodes){
	intt DIPHA = 8067171840,file_type = 8;
	sFile.write(reinterpret_cast<const char*>(&DIPHA), sizeof(intt));
	sFile.write(reinterpret_cast<const char*>(&file_type), sizeof(intt));
	sFile.write(reinterpret_cast<const char*>(&num_nodes), sizeof(intt));
	ifstream cFileIn("connections.txt");
	ifstream indFileIn("indices.txt");
	ifstream wFileIn("weight.txt");

	intt conn = 0, ind = 0, dis = 0;
	string line;
	while(getline(cFileIn, line)){
		intt u = stoll(line);		
		conn += u;
		sFile.write(reinterpret_cast<const char*>(&u), sizeof(intt));
	}
	while(getline(indFileIn, line)){
		intt u = stoll(line);
		ind++;
		sFile.write(reinterpret_cast<const char*>(&u), sizeof(intt));
	}
	while(getline(wFileIn, line)){
		double u = stod(line);
		dis++;
		double distance = static_cast<double>(u);
		sFile.write(reinterpret_cast<const char*>(&distance), sizeof(double));
	}

	indFileIn.close();
	wFileIn.close();
	cFileIn.close();
	// cout << conn << " " << ind << ' ' << dis << '\n';
}

void dump_pairs(intt u, vector<double>& avg_dist, vector<intt>& hop_dist, intt num_nodes){

	for(intt i = 1; i <= num_nodes; i++){

		double distance = avg_dist[i];
		if(distance == INT64_MAX)
			distance = 6;

		dFile << distance << " " << hop_dist[i] << " " << to_node[u] << " " << to_node[i] << '\n';
	}
}

void dijsktra(vector< vector< ii > >& adj, vector<intt>& dist, intt src, intt num_nodes){
	dist[src] = 0;
	// cout << "Source: " << src << '\n';
	priority_queue< ii , vector< ii >, greater< ii > > pq; // make priority queue for storing pairs(distance , vertex)
	pq.push(make_pair(0, src));

	while(!pq.empty()){
		ii u = pq.top(); // get the minimum weight element
		pq.pop();

		// for every edge u-v, update the distance of v from source
		for(intt i = 0; i < adj[u.second].size(); i++){
			ii v = adj[u.second][i];

			// update condition
			if(dist[v.second] > dist[u.second] + v.first){
				dist[v.second] = dist[u.second] + v.first;
				pq.push(make_pair(dist[v.second], v.second)); // push the new distance, vertex pair in the queue 
			}
		}
	}

	return;
}


bool bellman(vector< vector< ii > >& adj, vector<intt>& dist, intt src, intt num_nodes){
	dist[src] = 0;

	for(intt i = 1; i < num_nodes-1; i++){

		// for every edge
		for(intt u = 0; u <= num_nodes; u++){
			for(intt i = 0; i < adj[u].size(); i++){

				// update the distance of node v from source vertex
				ii v = adj[u][i];

				if(dist[v.second] > dist[u] + v.first){
					dist[v.second] = dist[u] + v.first;
				}
			}
		}
	}

	for(intt u = 0; u <= num_nodes; u++){
		for(intt i = 0; i < adj[u].size(); i++){

			// if you can find a shorter path from source to some vertex then report negative cycle
			ii v = adj[u][i];

			if(dist[v.second] > dist[u] + v.first){
				return false;
			}
		}
	}

	return true;
}

void johnson(vector< vector< ii > >& adj, vector< vector< ii > >& reverse_adj, intt num_nodes, bool dumping){

	// connect an extra vertex to every node to get graph G'
	for(intt i = 1; i <= num_nodes; i++){
		adj[0].push_back(make_pair(0, i));
	}

	// cout << "New node created.\n";

	vector<intt> dist(num_nodes+1,INT64_MAX);
	vector<intt> reverse_dist(num_nodes+1, INT64_MAX);
	vector<double> distance(num_nodes+1, 0.0);

	// check for negative weight cycle in new graph
	if(/*!bellman(adj, dist, 0, num_nodes)*/false){
		// cout << "Negative weight cycle found.\n Computation aborted.\n";
	}
	
	else{
		// cout << "No negative cycle.\n";
		// create new node weights
		vector<intt> h(num_nodes+1, 0);
		for(intt u = 0; u <= num_nodes; u++){
			h[u] = dist[u];
		}

		// create new edge weights such that all the weights are positive
		for(int u = 0; u <= num_nodes; u++){
			for(intt i = 0; i < adj[u].size(); i++){

				adj[u][i].first = adj[u][i].first + h[u] - h[adj[u][i].second];

			}
		}

		// run djikstra for every vertex
		// cout << "Starting..\n";
		for(intt u = 1; u <= num_nodes; u++){

			// check progress
			if(u%1000 == 0){
				cout << "Done till " << u <<"th node.\n";
			}
			
			fill(dist.begin(), dist.end(), INT64_MAX);
			fill(reverse_dist.begin(), reverse_dist.end(), INT64_MAX);

			dist[u] = reverse_dist[u] = 0;

			if(adj[u].size() != 0)
				dijsktra(adj, dist, u, num_nodes);

			if(reverse_adj.size() != 0)
				dijsktra(reverse_adj, reverse_dist, u, num_nodes);

			// get old edge/path weights back
			for(intt v = 1; v <= num_nodes; v++){
				if(dist[v] == INT64_MAX) 
					dist[v] = 6;
				if(reverse_dist[v] == INT64_MAX )
					reverse_dist[v] = 6;

				distance[v] = ((double)dist[v] + (double)reverse_dist[v])/2.0;
				// if(dist[v] == 5 or dist[v] == 6 or dist[v] == 3)
				// 	cout << dist[v] << " dest-> " << to_node[v] << " sourcce-> " << to_node[u] << '\n';	

			}

			// write the output to the file
			write_txt(u, distance, num_nodes);

			//write pairs if dumped == true
			if(dumping)
				dump_pairs(u, distance, dist, num_nodes);

		}
		// wFile.close();
		// indFile.close();
		// cFile.close();

		// write_sparse(num_nodes);

	} 
}

intt input(map<string,intt>& to_indices, map<intt,string>& to_node){
	string u; 
	intt edges = 0;
	int num_sources = 0;
	intt i = 1;
	in.resize(1);
	reverse_in.resize(1);
	string source, dest;
	while(true){
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
				source = u.substr(0, u.length()-1); // remove colon
				
				if(to_indices.find(source) == to_indices.end()){
					to_indices[source] = i;
					to_node[i] = source;
					vector< ii > tmp;
					in.push_back(tmp);
					reverse_in.push_back(tmp);
					i++;
				}

			}

			else{
				edges++;
				intt weight;
				dest = u;

				if(to_indices.find(dest) == to_indices.end()){
					to_indices[dest] = i;
					to_node[i] = dest;
					vector< ii > tmp;
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
	cout << "Edges: " << edges << '\n';
	return i-1; // number of distinct nodes
}


int main(int argc, char *argv[]){
	fastio;

	// for taking command line arguements
	int j = 1;

	// used to detect user option for dumping all the pairs
	bool dumping = false;

	if(argc < 5){
		cout << "[Usage]: " << "./johnson dataset_name input_filename output_filename sparse_output_filename\n";
		cout << "Options: \n";
		cout << "--dump_pairs -> dump all the reachable pairs into text file with the distance between them. Use for testing on a big file.\n";
		return 0;
	}

	else if(argc == 6 and (string)argv[1] == "--dump_pairs"){
		j++;
		dumping = true;
	}

	string dataset_name = argv[j++];
	string input_file = argv[j++];
	string output_file = argv[j++];
	string sparse_file = argv[j++];

	if(dumping)
		dFile.open("/home/deepak/Project/files/outputs/"+dataset_name+"/dumped.txt");

	iFile.open((string) input_file);
	oFile.open((string) output_file);
	sFile.open((string) sparse_file, ios::binary);

	intt num_nodes = input(to_indices, to_node); // take input
	cout << num_nodes << '\n';
	return 0;

	if(num_nodes == 1){
		oFile << (double) 0 << '\n';
		iFile.close();
		oFile.close();
		sFile.close();
		dFile.close();
		return 0;
	}

	// intt DIPHA = 8067171840,file_type = 7;
	// oFile.write(reinterpret_cast<const char*>(&DIPHA), sizeof(intt));
	// oFile.write(reinterpret_cast<const char*>(&file_type), sizeof(intt));
	// oFile.write(reinterpret_cast<const char*>(&num_nodes), sizeof(intt));


	// for(int i = 1; i <= num_nodes; i++){
	// 	cout << to_node[i] << ":\n";
	// 	for(int j = 0; j < in[i].size(); j++)
	// 		cout << to_node[in[i][j].second] << " "  << in[i][j].first << '\n';
	// }

	// cout << "\n\n";
	
	// for(int i = 1; i <= num_nodes; i++){
	// 	cout << to_node[i] << ":\n";
	// 	for(int j = 0; j < reverse_in[i].size(); j++)
	// 		cout << to_node[reverse_in[i][j].second] << " "  << reverse_in[i][j].first << '\n';
	// }


	johnson(in, reverse_in, num_nodes, dumping);

	iFile.close();
	oFile.close();
	sFile.close();
	dFile.close();

	return 0;
}
