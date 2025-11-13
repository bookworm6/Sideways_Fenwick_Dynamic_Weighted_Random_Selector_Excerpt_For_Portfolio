# Sideways_Fenwick_Dynamic_Weighted_Random_Selector_Excerpt_For_Portfolio
Excerpt from a research project with the Whitman Opti Lab optimizing a potential replacement for C++ std::discrete_distribution that supports dynamic weight updates. Although later outperformed by newer versions, the sideways_fenwick_selector demonstrates my programming and optimization skills. 

## Purpose
The sideways fenwick selector is designed for stochastic simulations, in which there is a set of possible chemical reactions with different probabilities of occurring. The simulation needs to select a reaction to occur and then update the probabilities of other reactions occurring based on changes in the amount of reactants. Since the selection and update operations need to be performed millions of times, it is important that they are as efficient as possible. 

## How to Run
### Performance Tests
The performance tests compare the sideways fenwick selector with two other libraries for dynamic weighted random selection developed by other research labs. If you would like to run the performance tests use `bash run_all_tests.sh`. However, be aware that the performance tests can take several hours to run and that accurate results require that you don't use your computer for other tasks while you run it. Also, factors such as your specific CPU, your compiler version, whether your laptop is plugged in, etc. can all effect the performance results. Alternatively, a set of results are already in the respository. Assuming you have python, pandas, matplotlib, and numpy installed, you can view a graph of the results by running performanceGrapher.py.

### Accuracy Tests 
If you would like to see a quick visual representation confirming that the sideways fenwick selector correctly selects elements in proportion to their weights, compile accuracyTest.cpp using `g++ -std=c++20 -O3 accuracyTest.cpp`. and then run it (`./a.out`). accuracyTest.cpp sets up a distribution in which there are ten elements with indexes 0 through 9, and assigns each elements their index as a weight. So the selector is 9 times more likely to select element 9 than element 1. It then selects an elements 100,000 times and writes the results to accuracyTestOutput.csv. You can view the distribution of results by running accuracyGrapher.py (python and matplotlib must be installed). It create a histogram (accuracyResults.png) of the selected element's index (which is equal to its weight in this example) and the number of times it was selected. 

## About the Sideways Fenwick Tree
One data structure we explored was a fenwick tree. However, our update function frequently needed to update elements that were extremely far apart in memory, leading to paging issues. This led to the sideways fenwick tree, which has better locality.

![Diagram of the sideways fenwick tree](/Sideways_Fenwick_Tree_Diagram.png)

The sideways fenwick tree is a 1 indexed binary tree where each node represents a weight, but stores the weight of its left subtree including its own weight. Since it is one idexed it possible to move from a child node to its parent using a right bitshift, to move from a parent to a left child using a left bitshift. and to move from a parent to a right child by using a left bitshift and then adding 1. This enables the binary representation of an index of a node gives the path to that node. When the update algorithm searches for the next node to update, it needs to go up the tree to find the first node that contains the previous node in its right subtree. It is possible to do this in O(1) time by by shifting the index right by the number of trailing ones in the index +1. 

![Diagram of the path of in-order traversal of a binary tree](/in_order_traversal.png)
The sideways fenwick tree is based off of the original Fenwick tree, which allows the user to find the sum of all of the elements between two indexes with logarithmic time complexity. The sideways fenwick’s selection algorithm is based on this ability. However, in the sideways fenwick, instead of finding the sum between two indexes, we can find the sum along the path of in-order traversal of a binary tree. For clarity, this path is numbered in the diagram above. This path is useful to us because any given left child will contain the sum of all weights between the first ancestor that it descended right from and itself (or, if the node is not in the right subtree of any of its ancestors, it will contain the sum of all weights between element 1 and itself). Any given right child will contain the sum of all weights between its parent and itself not including the weight of the parent. 

To randomly select a node based on weight, the selection algorithm generates a random number in the range [0,total weight). Then, it selects the first element in which the sum of the weights from element 1 to the element (including the weight of the element) is greater than the target. 



