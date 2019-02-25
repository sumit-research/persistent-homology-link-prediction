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
                     << "./script dataset_name train_set input_file threshold output_file database_loc \n";
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
        string input_file = argv[j++];
        double threshold = atof(argv[j++]);
        string output_file = argv[j++];
        string database_loc = argv[j++];

        char database_loc_proper[database_loc.size()];
        strcpy(database_loc_proper, database_loc.c_str());
        int rc = sqlite3_open(database_loc_proper, &database);

        tsFile.open((string)input_file);
        oFile.open((string)output_file);

        auto start = std::chrono::high_resolution_clock::now();
        intt num_nodes = input(train_set, in, reverse_in, to_indices, to_node, weighted, directed); // take input
                                
        
        double scores[8];
        string sources[2];
        int count_test_cases = 0;
        if (tsFile)
        {
                while (true)
                {
                        string label, source, dest;
                        tsFile >> source >> dest >> label;

                        if (tsFile.eof())
                        {
                                break; // break it end of file
                        }

                        else
                        {
                                auto start_ = std::chrono::high_resolution_clock::now();
                                string sources[2] = {source, dest};

                                intt hop_dist = get_hop_distance(sources);
                                intt comb_nbd_hop = hop_dist/2 + 1;
                                intt nbd_hop = hop_dist;

                                if(comb_nbd_hop == 0)
                                    comb_nbd_hop = 1;

                                to_ind.clear();
                                to_no.clear();
                                vector<vector<ii> > a_nbd = getNhop_database(dataset_name, source, comb_nbd_hop);
                                vector<vector<double> > pd_a = getPD(a_nbd, threshold);
                                callFunctions(sources, comb_nbd_hop, dataset_name, pd_a, scores, threshold, directed);

                                cout << "\n"
                                                     << source << "\t"
                                                     << dest << "\t"
                                                     << scores[0] << ","
                                                     << scores[1] << ","
                                                     << scores[2] << ","
                                                     << scores[3] << ","
                                                     << scores[4] << ","
                                                     << scores[5] << ","
                                                     << scores[6] << ","
                                                     << scores[7] << ","
                                                     << label << "\t";
								                oFile << "\n"
                                                      << source << "\t"
                                                      << dest << "\t"
                                                      << scores[0] << ","
                                                      << scores[1] << ","
                                                      << scores[2] << ","
                                                      << scores[3] << ","
                                                      << scores[4] << ","
                                                      << scores[5] << ","
                                                      << scores[6] << ","
                                                      << scores[7] << ","
                                                      << label << "\t";
                               
                                auto finish_ = std::chrono::high_resolution_clock::now();
                                chrono::duration<double> elapsed = finish_ - start_;

                                cout << "\nTime taken per pair: " << elapsed.count() << '\n';
                                count_test_cases++;
                                if(count_test_cases%10 == 0)
                                    cout << count_test_cases << '\n';


                        }

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
