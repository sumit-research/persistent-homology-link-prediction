#include "functions.h"
#include <chrono>
using namespace std;
#define intt int64_t
#define ii pair<double, intt>
#define fastio                    \
        ios_base::sync_with_stdio(0); \
        cin.tie(0);                   \
        cout.tie(0)

int main(int argc, char *argv[])
{
        // fastio;
        bool directed = false;
        bool weighted = false;
        if (argc < 7)
        {
                cout << "[Usage]: "
                     << "./script dataset_name train_set test_set threshold output_file database_loc \n";
                return 0;
        }

        int j = 1;

        if((string)argv[1] == "--directed"){
            j++;
            directed = true;
        }

        if((string)argv[2] == "--weighted"){
            j++;
            weighted = true;
        }

        string dataset_name = argv[j++];
        string train_set = argv[j++];
        string test_set = argv[j++];
        // int nbd_hop = atoi(argv[j++]);
        // int comb_nbd_hop = atoi(argv[j++]);
        double threshold = atof(argv[j++]);
        string output_file = argv[j++];
        string database_loc = argv[j++];

        // string database_loc = "./database.db";
        char database_loc_proper[database_loc.size()];
        strcpy(database_loc_proper, database_loc.c_str());
        int rc = sqlite3_open(database_loc_proper, &database);

        tsFile.open((string)test_set);
        oFile.open((string)output_file);
        // sFile.open((string) sparse_file, ios::binary);

        auto start = std::chrono::high_resolution_clock::now();

        intt num_nodes = input(train_set, in, reverse_in, to_indices, to_node, weighted, directed); // take input
        double scores[8];
        string sources[2];
        int count_test_cases = 0;
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
                                // source = "17731";
                                // dest = "12806";
                                auto start_ = std::chrono::high_resolution_clock::now();
                                string sources[2] = {source, dest};
                                intt hop_dist = get_hop_distance(sources);
                                // cout << "here79\n";
                                intt comb_nbd_hop = hop_dist/2 + 1;
                                intt nbd_hop = hop_dist;


                                if(comb_nbd_hop == 0)
                                    comb_nbd_hop = 1;

                                vector<string> src_nbd = just_getNhop_database(dataset_name, source, nbd_hop);
                                bool isFoundDest = false;
                                // cout<<"\nSource: "<<source<<"\t NbdSize = "<<src_nbd.size()<<"\n";
                                // iargvnt a;
                                // cin>>a;
                                to_ind.clear();
                                to_no.clear();
                                vector<vector<ii> > a_nbd = getNhop_database(dataset_name, source, comb_nbd_hop);
                                // cout << a_nbd.size() << '\n';
                                // cin >> a;
                                // cout << "\n390\n";
                                vector<vector<double> > pd_a = getPD(a_nbd, threshold);
                                // cout << "here\n";

                                for (size_t i = 0; i < src_nbd.size(); i++)
                                {
                                        // cout << "\n"
                                        //   << i << "," << src_nbd[i] << "\n";

                                        if (src_nbd[i] == dest)
                                        {
                                                isFoundDest = true;
                                                break;
                                        }
                                }
                                // cout<<"\nisFound: "<<isFoundDest<<"\n";
                                // cout << "\n"
                                //   << source << "\t"
                                //   << dest << "\n";
                                if (isFoundDest)
                                {

                                        for (size_t i = 0; i < src_nbd.size(); i++)
                                        {
                                                // vector<string> sources;
                                                sources[0] = source;
                                                sources[1] = src_nbd[i];
                                                // sources[1] = "21281";
                                                
                                                if(src_nbd[i] == source)
                                                    continue;

                                                // sources.push_back("1033");
                                                // sources.push_back("1034");
                                                // cout << "\n"<<sources[0] << "\t" << src_nbd[i] << "\n";

                                                callFunctions(sources, comb_nbd_hop, dataset_name, pd_a, scores, threshold, directed);
                                                // cout << "hereaaa\n";
                                                cout << "\n"
                                                     << source << "\t"
                                                     << dest << "\t"
                                                     << sources[1] << ":"
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
                                                //  auto start_ = std::chrono::high_resolution_clock::now(); << scores[12] << ","
                                                //   << scores[13] << ","
                                                //   << scores[14] << ","
                                                //   << scores[15] << "\t";
                                                // return 0;
                                        }

                                }
                                else
                                {
                                        // cout << "\n"
                                        //   << source << "\t" << dest << "\t"
                                        //   << "inf";
                                        oFile << "\n"
                                              << source << "\t" << dest << "\t"
                                              << "inf";
                                }
                                auto finish_ = std::chrono::high_resolution_clock::now();
                                chrono::duration<double> elapsed = finish_ - start_;

                                cout << "\nTime taken per pair: " << elapsed.count() << '\n';
                                count_test_cases++;
                                if(count_test_cases%10 == 0)
                                    cout << count_test_cases << '\n';

                                src_nbd.clear();

                        }
                        // return 0;

                }
        }

        tsFile.close();
        oFile.close();
        sqlite3_close(database);

        auto finish = std::chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = finish - start;

        cout << "\nTime taken: " << elapsed.count() << '\n';

        return 0;
}
