#!/bin/bash

g++ -std=c++17 johnson.cpp -o johnson
./johnson --dump_pairs cora /home/deepak/Project/files/data/cora/data.txt /home/deepak/Project/files/outputs/cora/apsp_full.txt /home/deepak/Project/files/outputs/cora/dumped.txt
#python3 createDB.py ../../files/outputs/cora/database.db ../../files/outputs/cora/dumped.txt
g++ -std=c++17 -pthread script.cpp -l sqlite3 -O3 -o run_script
./run_script cora /home/deepak/Project/files/data/cora/data.txt  /home/deepak/Project/files/data/cora/testset.txt 3 5 /home/deepak/Project/files/outputs/cora/testOutput_3.txt