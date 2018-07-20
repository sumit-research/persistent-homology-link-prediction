# ShapeOfYou
This repository contains all the code written for a project with IBM. In this project we use concepts from algebraic topology 
in the link prediction problem. TDA \- Topological Data Analysis, is an emerging data analytics tool. One of the common methods used within the context of TDA is **Persistent Homology**.

## Experiments
1. **Experiment 1**: We generate all the possible pairs for the given graph file and store those pairs with the distance in a file. We calculate all the scores for all those pairs sequentially. 

2. **Experiment 2**: In this experiment, we take a random sample of 100 edges from the graph. For each edge, we remove that edge from the graph and make a new graph, and do all the computation using that graph. 

3. **Experiment 3**: In this experiment we make a random sample from the graph. Random sample should contain some positive and some negative examples. Positive examples mean that those pair of nodes have an edge between them in the graph, and in negative examples they don't have an edge. Now we use experiment 2 for positive examples(removing that edge and then computing scores) and for negative edges we just compute the scores directly. Then we train a machine learning model on those scores.

## Usage
### I/O format
All the code need a graph file and at least one output file as command line arguements. 
**Graph File** \- It will be a .txt file. If there is a node _a_ with 5 out edges to nodes b,c,d,e then it should be represented as:
```
a:
b
c
d
e
```
Similarly for all other nodes.

**pair_data** \- This file will be produced by using --dump_pairs option in johnson.cpp. The structure is:
```
distance(a,b) node_a node_b
```
If d(a,b) represents shortest distance from node _a_ to _b_ then:
distance(a,b) = [d(a,b) + d(b,a)]/2

**DIPHA files**
Some of the codes(johnson.cpp) generate DIPHA distance matrix files. You can read more about them [here](https://github.com/DIPHA/dipha).
We feed these files to dipha program and it gives a DIPHA persistent diagram file.

### Details
There is one main project directory, which has two folders:
1. code \- contains source code of the project
   - src_server \- This folder was created to modify the code as per needs of the server.
     - **baseline.cpp**
       - ```./baseline graph_filename output_filename source_node``` \- Takes a node id as an input, it produces a list of AA and MW score with all other nodes. The list is ranked from most similar to least similar.
       - ```./baseline input_filename output_filename source_node node``` \- Takes two node ids as input, it produces AA and MW scores between those two nodes.
       
     - **n_hop_multi.cpp**
       - ```./n_hop_multi graph_filename output_filename source_1 number_of_hops``` \- Takes a node id and number of        hops(**n**) as input, it outputs **n**-hop neighborhood of that individual node.
		   - ```./n_hop_multi graph_filename output_filename source_1 source_2 number_of_hops``` \- Takes two node ids and number of hops(**n**) as input, it outputs _combined_ **n**-hop neighborhood for given nodes. If two source nodes are provided then it also outputs a complete graph formed by set of nodes involved in combined neighborhood.
		   - Options: \-\-remove: Used when the input data is modified by removing an edge. This option stores all the results in a new folder.
       
     - **johnson.cpp**
       - ```./johnson graph_filename output_filename sparse_output_filename``` \- This outputs full and sparse distance matrix for the given graph.
       - Options: \-\-dump_pairs: Using this you can dump all the pairs and distance between them in a file.
       
     - **compare_diagram.py**
       - ```python3 compare_diagram.py dgm1_file dgm2_file p dimension``` \- Takes two DIPHA persistent diagram files, and compute Wasserstein and Bottleneck distance between them. **p** is dimension of the norm used in Wassertein distance, and **dimension** is dimension of the topological feature w.r.t which all the computation is done.
     
     - **get_persDiag.py**
       - ```python3 script.py graph_filename node_a n``` \- Takes a node id and number of hops(**n**) as input, it ouputs a DIPHA persistent diagram file for **n**\-hop neighborhood of given node.
       - ```python3 script.py data_file node_a node_b n``` \- Takes two node ids and number of hops(**n**) as input, it ouputs a DIPHA persistent diagram file for combined **n**\-hop neighborhood of given nodes.
       
      - **script.py**
        - ```python3 script.py graph_filename pair_data output_file n``` - Takes pair_data and number of hops(**n**) as input and outputs all the permutations of Wasserstein and Bottleneck distance scores corresponding to **n**-hop neighborhood for all the pairs in pair_data file. 
        - If the output_file already exists, then this script will resume from where it was stopped last time.
       
      - **script_2.py**
        - ```python3 script.py graph_filename edges_out output_file n sample_size``` - Takes number of hops(**n**) and sample size as input. This script selects _sample\_size_ edges and for an edge **(a,b)** gives rank of node b in the rank list of node a w.r.t to each metric. All the computation is done after removing the already existing edge **(a,b)** from the graph.
        - If the edges_out already exists, then this script will give output for all the edges in that file.
        - If the output_file already exists, then this script will resume from where it was stopped last time.
        - **Precaution** - edges\_out and output\_file always exist in pair. So when resuming computation, you should be careful to provide correct pair.
      
      - **mrr.py**
        - ```python3 mrr.py csv_file``` - Takes output file of script_2.py and generates **Mean Reciprocal Rank** and **Precision** at 5,10,20,50,100 for the given ranks and appends the result to the given csv_file.
        
      - **make_sample.py**
        - ```python3 make_sample.py graph_filename sample_out_file num_positive_samples num_negative_samples``` - Takes number of positive and negative examples you want to generate and outputs the sample in _sample\_out\_file_. Here positive example means pair of nodes having an edge between them in the given graph file. 
   - src_v2 - This folder was created to run the code on local laptop. Files are same as src_server.
   
2. files \- This folder contains data and output files. 
   - **Data** \- This folder contains all the datasets, experimental sample test(obtained from those datasets)
   - **Outputs** \- This folder contains all the results and intermediate outputs of all experiments. It has many folders with the folder name as the dataset name. 
       - experiment_1: This contains files of type _output\_hop.csv_, where hop is a number.
       - experiment_2: This contains files of type _output\_hop\_expermentNum.csv_, where hop is the hop number and experimentNum is id of experiment. 
       - experiment_3: This contains files of type _mloutput_num.csv_, where num is experiment number.
       - n_hop: There are many folders of this type. These folders contain all the intermediate outputs like apsp distance matrix, n hop neighborhood data and dipha files for that value of hop.
         - apsp_complete_full_node1_node2: This file contains complete graph made by n_hop neighborhood of node1 and node2.
         - apsp_full_node1: This file contains full distance matrix for n_hop neighborhood of node1. Another variant of this file is apsp_sparse_node1, which contains sparse distance matrix.
         - apsp_full_node1_node2: This file contains full distance matrix for combined n_hop neighborhood of node1 and node2. Another variant of this file is apsp_sparse_node1_node2, which contains sparse distance matrix.
         - n_hop_node1: This file contains n_hop neighborhood subgraph for node1.
         - n_hop_node1_node2: This file contains combined n_hop neighborhood subgraph for node1 and node2.
         - dipha_node1: This file is a DIPHA persistent diagram file for n_hop neighborhood of node1.
         - dipha_node1_node2: This file is a DIPHA persistent diagram file for n_hop neighborhood of node1 and node2.
       - removed_edge_hop: These folders are used to store intermediate results when we are working on experiment 2, i.e. when we do all the computation after removing an edge from the graph. The file names in these folders are similar to file names in n_hop folders. 
