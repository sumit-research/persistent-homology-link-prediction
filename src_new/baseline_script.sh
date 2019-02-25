#!/bin/bash

echo "Computing baseline for cora..."
./baseline --directed ../../files/new_data/cora/cora.train ../../files/new_data/cora/cora.test ../../files/new_outputs/pacm_scores/cora.aamw 2 

echo "Computing baseline for ATC..."
./baseline --directed ../../files/new_data/atc/atc.train ../../files/new_data/atc/atc.test ../../files/new_outputs/pacm_scores/atc.aamw 2

echo "Computing baseline for DC..."
./baseline ../../files/new_data/dc/dc.train ../../files/new_data/dc/dc.test ../../files/new_outputs/pacm_scores/dc.aamw 2

echo "Computing baseline for euroad..."
./baseline ../../files/new_data/euroad/euroad.train ../../files/new_data/euroad/euroad.test ../../files/new_outputs/pacm_scores/euroad.aamw 2

echo "Computing baseline for figeyes..."
./baseline --directed ../../files/new_data/figeyes/figeyes.train ../../files/new_data/figeyes/figeyes.test ../../files/new_outputs/pacm_scores/figeyes.aamw 2

echo "Computing baseline for power..."
./baseline ../../files/new_data/power/power.train ../../files/new_data/power/power.test ../../files/new_outputs/pacm_scores/power.aamw 2

echo "Computing baseline for yeast..."
./baseline ../../files/new_data/yeast/yeast.train ../../files/new_data/yeast/yeast.test ../../files/new_outputs/pacm_scores/yeast.aamw 2

echo "Computing baseline for arxiv..."
./baseline ../../files/new_data/arxiv/arxiv.train ../../files/new_data/arxiv/arxiv.test ../../files/new_outputs/pacm_scores/arxiv.aamw 2

echo "Computing baseline for twitter..."
./baseline --directed ../../files/new_data/arxiv/arxiv.train ../../files/new_data/arxiv/arxiv.test ../../files/new_outputs/pacm_scores/arxiv.aamw 2

echo "Done"


