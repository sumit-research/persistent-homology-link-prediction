#!/bin/bash

g++ -std=c++17 johnson.cpp -o johnson
./johnson --dump_pairs bitcoin /home/deepak/Project/files/data/bitcoin/data.txt /home/deepak/Project/files/outputs/bitcoin/apsp_full.txt /home/deepak/Project/files/outputs/bitcoin/dumped.txt
python3 createDB.py ../../files/outputs/bitcoin/database.db ../../files/outputs/bitcoin/dumped.txt
g++ -std=c++17 -pthread script.cpp -l sqlite3 -O3 -o run_script
./run_script bitcoin /home/deepak/Project/files/data/bitcoin/data.txt  /home/deepak/Project/files/data/bitcoin/testset.txt 3 5 /home/deepak/Project/files/outputs/bitcoin/testOutput_3.txt
