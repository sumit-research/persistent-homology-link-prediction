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
        if (argc < 5)
        {
                cout << "[Usage]: "
                     << "./script dataset_name train_set test_set database_loc \n";
                return 0;
        }

        int j = 1;

        if((string)argv[1] == "--directed") {
                j++;
                directed = true;
        }

        if((string)argv[2] == "--weighted") {
                j++;
                weighted = true;
        }

        string dataset_name = argv[j++];
        string train_set = argv[j++];
        string test_set = argv[j++];
        string database_loc = argv[j++];

        // string database_loc = "./database.db";
        char database_loc_proper[database_loc.size()];
        strcpy(database_loc_proper, database_loc.c_str());
        int rc = sqlite3_open(database_loc_proper, &database);

        tsFile.open((string)test_set);
        // sFile.open((string) sparse_file, ios::binary);

        auto start = std::chrono::high_resolution_clock::now();

        intt num_nodes = input(train_set, in, reverse_in, to_indices, to_node, weighted, directed); // take input
        double scores[8];
        string sources[2];
        intt count_full = 0;
        intt total_neg = 0;
        intt total_nbd = 0;

        if (tsFile)
        {
                while (true)
                {
                        string source, dest, label;
                        // cin >> u;
                        tsFile >> source >> dest >> label; // read from file

                        if (tsFile.eof())
                        {
                                break; // break it end of file
                        }

                        else
                        {
                                // source = "19697";
                                // dest = "24966";
                                if(label == "1")
                                    continue;

                                total_neg++;
                                string sources[2] = {source, dest};
                                intt hop_dist = get_hop_distance(sources);
                                intt comb_nbd_hop = hop_dist/2 + 1;
                                intt nbd_hop = hop_dist;


                                if(comb_nbd_hop == 0)
                                        comb_nbd_hop = 1;

                                if(nbd_hop == 100)
                                    count_full++;

                                intt src_nbd = get_nbd_size(source, nbd_hop);
                                total_nbd += src_nbd;
                                // bool isFoundDest = false;

                                to_ind.clear();
                                to_no.clear();

                                cout << source << " " <<  dest << " " << hop_dist << '\n';
                                // cout << "\nNeighborhood hop distance: " << nbd_hop;
                                cout << "\nNeighborhood Size: " << src_nbd << "\n\n";

                        }
                        // return 0;

                }
        }

        tsFile.close();
        sqlite3_close(database);

        auto finish = std::chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = finish - start;

        cout << "Number of pairs having no path: " << count_full << " out of " << total_neg << '\n';
        cout << total_nbd/total_neg << '\n';
        // cout << "\nTime taken: " << elapsed.count() << '\n';

        return 0;
}
