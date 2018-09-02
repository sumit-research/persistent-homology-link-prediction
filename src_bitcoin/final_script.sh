#!/bin/bash

g++ -std=c++17 johnson.cpp -o johnson
./johnson --dump_pairs moreno /home/deepak/Project/files/data/moreno/data.txt /home/deepak/Project/files/outputs/moreno/apsp_full.txt /home/deepak/Project/files/outputs/moreno/dumped.txt
python3 createDB.py ../../files/outputs/moreno/database.db ../../files/outputs/moreno/dumped.txt
g++ -std=c++17 -pthread script.cpp -l sqlite3 -O3 -o run_script
./run_script moreno /home/deepak/Project/files/data/moreno/data.txt  /home/deepak/Project/files/data/moreno/testset.txt 5 2 /home/deepak/Project/files/outputs/moreno/testOutput_5_2.txt
