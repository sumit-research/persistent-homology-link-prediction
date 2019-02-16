#!/bin/bash

echo "Computing features for training set..."


echo "Computing features for cora..."
./run_script_reg --directed cora ../../files/new_data/cora/cora.train ../../files/new_data/regression/cora.reg.train 4 ../../files/new_outputs/cora/cora.features.train ../../files/new_outputs/cora/database.db

echo "Computing features for ATC..."
./run_script_reg --directed atc ../../files/new_data/atc/atc.train ../../files/new_data/regression/atc.reg.train 4 ../../files/new_outputs/atc/atc.features.train ../../files/new_outputs/atc/database.db

echo "Computing features for DC..."
./run_script_reg dc ../../files/new_data/dc/dc.train ../../files/new_data/regression/dc.reg.train 4 ../../files/new_outputs/dc/dc.features.train ../../files/new_outputs/dc/database.db

# echo "Computing features for euroad..."
# ./run_script_reg euroad ../../files/new_data/euroad/euroad.train ../../files/new_data/regression/euroad.reg.train 4 ../../files/new_outputs/euroad/euroad.features.train ../../files/new_outputs/euroad/database.db

# echo "Computing features for figeyes..."
# ./run_script_reg --directed figeyes ../../files/new_data/figeyes/figeyes.train ../../files/new_data/regression/figeyes.reg.train 4 ../../files/new_outputs/figeyes/figeyes.features.train ../../files/new_outputs/figeyes/database.db

echo "Computing features for power..."
./run_script_reg power ../../files/new_data/power/power.train ../../files/new_data/regression/power.reg.train 4 ../../files/new_outputs/power/power.features.train ../../files/new_outputs/power/database.db

echo "Computing features for yeast..."
./run_script_reg yeast ../../files/new_data/yeast/yeast.train ../../files/new_data/regression/yeast.reg.train 4 ../../files/new_outputs/yeast/yeast.features.train ../../files/new_outputs/yeast/database.db

# echo "Computing features for arxiv..."
# ./run_script_reg arxiv ../../files/new_data/arxiv/arxiv.train ../../files/new_data/regression/arxiv.reg.train 4 ../../files/new_outputs/arxiv/arxiv.features.train ../../files/new_outputs/arxiv/database.db

# echo "Done"

echo "Computing features for test set..."

echo "Computing features for cora..."
./run_script_reg --directed cora ../../files/new_data/cora/cora.train ../../files/new_data/regression/cora.reg.final.test 4 ../../files/new_outputs/regressionResults/cora.features.test ../../files/new_outputs/cora/database.db

echo "Computing features for ATC..."
./run_script_reg --directed atc ../../files/new_data/atc/atc.train ../../files/new_data/regression/atc.reg.final.test 4 ../../files/new_outputs/regressionResults/atc.features.test ../../files/new_outputs/atc/database.db

echo "Computing features for DC..."
./run_script_reg dc ../../files/new_data/dc/dc.train ../../files/new_data/regression/dc.reg.final.test 4 ../../files/new_outputs/regressionResults/dc.features.test ../../files/new_outputs/dc/database.db

# echo "Computing features for euroad..."
# ./run_script_reg euroad ../../files/new_data/euroad/euroad.train ../../files/new_data/regression/euroad.reg.final.test 4 ../../files/new_outputs/regressionResults/euroad.features.test ../../files/new_outputs/euroad/database.db

# echo "Computing features for figeyes..."
# ./run_script_reg --directed figeyes ../../files/new_data/figeyes/figeyes.train ../../files/new_data/regression/figeyes.reg.final.test 4 ../../files/new_outputs/regressionResults/figeyes.features.test ../../files/new_outputs/figeyes/database.db

echo "Computing features for power..."
./run_script_reg power ../../files/new_data/power/power.train ../../files/new_data/regression/power.reg.final.test 4 ../../files/new_outputs/regressionResults/power.features.test ../../files/new_outputs/power/database.db

echo "Computing features for yeast..."
./run_script_reg yeast ../../files/new_data/yeast/yeast.train ../../files/new_data/regression/yeast.reg.final.test 4 ../../files/new_outputs/regressionResults/yeast.features.test ../../files/new_outputs/yeast/database.db

# echo "Computing features for arxiv..."
# ./run_script_reg arxiv ../../files/new_data/arxiv/arxiv.train ../../files/new_data/regression/arxiv.reg.final.test 4 ../../files/new_outputs/regressionResults/arxiv.features.test ../../files/new_outputs/arxiv/database.db
