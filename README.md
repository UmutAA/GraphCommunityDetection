A C program for detecting communities in undirected graphs using multiple algorithms, including **Louvain**, **Fast Greedy**, and custom **Structural Connectivity Partition (SCP) & Fast Louvain** methods. A test data (Zachary Karate Club) is present inside the zip file.

## Features

- Load graph data from a text file
- Represent graphs using adjacency lists
- Compute modularity and community structure
- Compare Louvain, Fast Greedy, SCP and Fast Louvain algorithms
- Measure execution time for each algorithm

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/UmutAA/GraphCommunityDetection
    ```
2. Navigate to the project directory:
    ```bash
    cd GraphCommunityDetection
    ```
3. Compile the program (Windows):
    ```bash
    gcc -o community_detection main.c
    ```

## Usage
1. Prepare a graph file (data.txt) in the format: [source destination]
2. Run the program:
    ```bash
    main.exe
    ```
3. The program will output modularity, number of communities, and execution time for each algorithm.

## Algorithms Implemented
- Louvain: Detects communities by maximizing modularity iteratively.

- Fast Greedy: Greedy optimization of modularity by merging communities.

- SCP (Structural Connectivity Partition): Custom algorithm based on strong-edge thresholds and common neighbors.

- Fast Louvain: A faster version of Louvain Phase 1. Sorts the nodes in a descending order of their degrees and assigning each node to a community by modularity difference.