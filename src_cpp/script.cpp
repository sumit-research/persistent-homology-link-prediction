#include "functions.h"
using namespace std;
#define intt int64_t
#define ii pair<intt, intt>
#define fastio                    \
	ios_base::sync_with_stdio(0); \
	cin.tie(0);                   \
	cout.tie(0)

int main(int argc, char *argv[])
{
	fastio;

	if (argc < 9)
	{
		cout << "[Usage]: "
			 << "./script dataset_name train_set test_set nbd_hop comb_nbd_hop threshold output_file database_loc \n";
		return 0;
	}
	string dataset_name = argv[1];
	string train_set = argv[2];
	string test_set = argv[3];
	int nbd_hop = atoi(argv[4]);
	int comb_nbd_hop = atoi(argv[5]);
	double threshold = atof(argv[6]);
	string output_file = argv[7];
	string database_loc = argv[8];

	// string database_loc = "./database.db";
	char database_loc_proper[database_loc.size()];
	strcpy(database_loc_proper, database_loc.c_str());
	int rc = sqlite3_open(database_loc_proper, &database);

	tsFile.open((string)test_set);
	oFile.open((string)output_file);
	// sFile.open((string) sparse_file, ios::binary);
	intt num_nodes = input(train_set, in, reverse_in, to_indices, to_node); // take input
	double scores[8];
	string sources[2];
	if (tsFile)
	{
		while (true)
		{
			string source, dest;
			// cin >> u;
			tsFile >> source >> dest; // read from file

			if (tsFile.eof())
			{
				break; // break it end of file
			}

			else
			{
				vector<string> src_nbd = just_getNhop_database(dataset_name, source, nbd_hop);
				bool isFoundDest = false;
				// cout<<"\nSource: "<<source<<"\t NbdSize = "<<src_nbd.size()<<"\n";
				// int a;
				// cin>>a;
				to_ind.clear();
				to_no.clear();
				vector<vector<ii>> a_nbd = getNhop_database(dataset_name, source, comb_nbd_hop);
				// cin >> a;
				// cout << "\n390\n";
				vector<vector<double>> pd_a = getPD(a_nbd, threshold);

				for (size_t i = 0; i < src_nbd.size(); i++)
				{
					// cout << "\n"
					// 	 << i << "," << src_nbd[i] << "\n";

					if (src_nbd[i] == dest)
					{
						isFoundDest = true;
						break;
					}
				}
				// cout<<"\nisFound: "<<isFoundDest<<"\n";
				cout << "\n"
					 << source << "\t"
					 << dest << "\n";
				if (isFoundDest)
				{

					for (size_t i = 0; i < src_nbd.size(); i++)
					{
						// vector<string> sources;
						sources[0] = source;
						sources[1] = src_nbd[i];
						// sources.push_back("1033");
						// sources.push_back("1034");
						// cout << "\n"<<source << "\t" << src_nbd[i] << "\n";
						callFunctions(sources, comb_nbd_hop, dataset_name, pd_a, scores, threshold);
						// cout << "\n"
						// 	 << source << "\t"
						// 	 << dest << "\t"
						// 	 << src_nbd[i] << ":"
						// 	 << scores[0] << ","
						// 	 << scores[1] << ","
						// 	 << scores[2] << ","
						// 	 << scores[3] << ","
						// 	 << scores[4] << ","
						// 	 << scores[5] << ","
						// 	 << scores[6] << ","
						// 	 << scores[7] << ","
						// 	 << scores[8] << ","
						// 	 << scores[9] << ","
						// 	 << scores[10] << ","
						// 	 << scores[11] << ","
						// 	 << scores[12] << ","
						// 	 << scores[13] << ","
						// 	 << scores[14] << ","
						// 	 << scores[15] << "\t";
						oFile << "\n"
							  << source << "\t"
							  << dest << "\t"
							  << src_nbd[i] << ":"
							  << scores[0] << ","
							  << scores[1] << ","
							  << scores[2] << ","
							  << scores[3] << ","
							  << scores[4] << ","
							  << scores[5] << ","
							  << scores[6] << ","
							  << scores[7] << "\t";
						//   << scores[8] << ","
						//   << scores[9] << ","
						//   << scores[10] << ","
						//   << scores[11] << ","
						//   << scores[12] << ","
						//   << scores[13] << ","
						//   << scores[14] << ","
						//   << scores[15] << "\t";
					}
					// break;
				}
				else
				{
					// cout << "\n"
					// 	 << source << "\t" << dest << "\t"
					// 	 << "inf";
					oFile << "\n"
						  << source << "\t" << dest << "\t"
						  << "inf";
				}

				src_nbd.clear();
				// break;
			}
		}
	}

	tsFile.close();
	oFile.close();
	sqlite3_close(database);

	return 0;
}
