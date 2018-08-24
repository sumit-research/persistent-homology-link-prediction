#!/bin/bash

./johnson --dump_pairs cora /home/deepak/Project/files/data/cora/data.txt /home/deepak/Project/files/outputs/cora/apsp_full.txt /home/deepak/Project/files/outputs/cora/dumped.txt
python3 createDB.py ../../files/outputs/cora/database.db ../../files/outputs/cora/dumped.txt