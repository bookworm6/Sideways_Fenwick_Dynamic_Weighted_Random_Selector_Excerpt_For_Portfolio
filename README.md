# Sideways_Fenwick_Dynamic_Weighted_Random_Selector_Excerpt_For_Portfolio
Excerpt from a research project with the Whitman Opti Lab optimizing a potential replacement for C++ std::discrete_distribution that supports dynamic weight updates. Although later outperformed by newer versions, the sideways_fenwick_selector demonstrates my programming and optimization skills. 

It is designed for stochastic simulations, in which there is a set of possible chemical reactions with different probabilities of occuring. The simulation needs to select a reaction to occur and then update the probabilities of other reactions occuring based on changes in the amount of reactants. Since the selection and update operations need to be perfomred millions of times, it is important that they are as efficient as possible. 

One data structure we explored was a fenwick tree. However, our update function frequently needed to update elements that were extremely far apart in memory, leading to paging issues. This led to the sideways fenwick tree, which has better locality.

The sideways fenwick tree is a 1 indexed binary tree where each node represents a weight, but stores the weight of its left subtree including its own weight. 

It is one indexed, which makes it possible to move from a child node to its parent using a right bitshift, to move from a parent to a left child using a left bitshift. and to move from a parent to a right child by using a left bitshift and then adding 1. This enables the binary representation of an index of a node to act as a map to that node. 

$$\documentclass[tikz,border=2mm]{standalone}
\usepgflibrary{shapes.geometric}
\begin{document}
\begin{tikzpicture}[
  level distance=3cm,
  level 1/.style={sibling distance=24cm},
  level 2/.style={sibling distance=12cm},
  level 3/.style={sibling distance=6cm},
  every node/.style={circle,draw,minimum size=12mm,inner sep=0pt}
]
\node [ellipse, draw, align=center]{Weight: O\\Stored: A+B+C+D+I+J+M+O\\Index:1\\Binary Index:0001}
  child {node[ellipse, draw, align=center] {Weight: M\\Stored: A+B+I+M\\Index:2\\Binary Index:0010}
    child {node[ellipse, draw, align=center] {Weight: I\\Stored: A+I\\Index:4\\Binary Index:0100}
        child{node[ellipse, align=center] {Weight: A\\Stored: A\\Index:8\\Binary Index:1000}}
        child{node[ellipse, align=center] {Weight: B\\Stored: B\\Index:9\\Binary Index:1001}}
    }
    child {node[ellipse, draw, align=center] {Weight: J\\Stored: C+J\\Index:5\\Binary Index:0101}
        child{node[ellipse, align=center] {Weight: C\\Stored: C\\Index:10\\Binary Index:1010}}
        child{node[ellipse, align=center] {Weight: D\\Stored: D\\Index:11\\Binary Index:1011}}
    }
  }
  child {node[ellipse, draw, align=center] {Weight: N\\Stored: E+F+K+N\\Index:3\\Binary Index:0011}
    child {node[ellipse, draw, align=center] {Weight: K\\Stored: E+K\\Index:6\\Binary Index:0110}
        child{node[ellipse, align=center] {Weight: E\\Stored: E\\Index:12\\Binary Index:1100}}
        child{node[ellipse, align=center] {Weight: F\\Stored: F\\Index:13\\Binary Index:1101}}
    }
    child {node[ellipse, draw, align=center] {Weight: L\\Stored: G+L\\Index:7\\Binary Index:0111}
        child{node[ellipse, align=center] {Weight: G\\Stored: G\\Index:14\\Binary Index:1110}}
        child{node[ellipse, align=center] {Weight: H\\Stored: H\\Index:15\\Binary Index:1111}}
    }
  };
\end{tikzpicture}
\end{document}$$


The sideways fenwick tree is based off of the original Fenwick tree, which allows the user to find the sum of all of the elements between two indexes with logarithmic time complexity. The sideways fenwick’s selection algorithm is based on this ability. However, in the sideways fenwick, instead of finding the sum between two indexes, we can find the sum along the path of in-order traversal of a binary tree. For clarity, this path is numbered using element numbers. This path is useful to us because any given left child will contain the sum of all weights between the first ancestor that it descended right from and itself (or, if the node is not in the right subtree of any of its ancestors, it will contain the sum of all weights between element 1 and itself). Any given right child will contain the sum of all weights between its parent and itself not including the weight of the parent. 

To randomly select a node based on weight, the selection algorithm generates a random number in the range [0,total weight). Then, it selects the first element in which the sum of the weights from element 1 to the element (including the weight of the element) is greater than the target. 



