#!/bin/bash

compile=(
 'g++ -std=c++20 -I../../lib -O3 -march=native "-DWRSLIB=complete_exponential_leaf_sum_tree<>" "-DWEIGHTNUM=${weightNums[$i]}" -o test$i uniform_changing.cpp'
'g++ -std=c++20 -I../../lib -O3 -march=native "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=${weightNums[$i]}" -o test$i uniform_changing.cpp'
'g++ -std=c++20 -I../../lib -O3 -march=native "-DWRSLIB=fast_random_selector<>" "-DWEIGHTNUM=${weightNums[$i]}" -o test$i uniform_changing.cpp'
'g++ -std=c++20 -I../../lib -O3 -march=native "-DWRSLIB=BucketMethod" "-DWEIGHTNUM=${weightNums[$i]}" -o test$i uniform_changing.cpp'
'g++ -std=c++20 -I../../lib -O3 -march=native "-DWRSLIB=sampling::DynamicProposalArray" "-DWEIGHTNUM=${weightNums[$i]}" -o test$i uniform_changing.cpp'
'g++ -std=c++20 -I../../lib -O3 -march=native "-DWRSLIB=complete_exponential_leaf_sum_tree<>" "-DWEIGHTNUM=${weightNums[$i]}" -o test$i uniform_changing.cpp'


)

names=(
  'leafsum_final'
  'Sideways_Fenwick_Selector'
  'Heap_original'
  'BucketMethod'
  'DynamicProposalArray'
)

for ((idx=0; idx<${#compile[@]}; idx++)); do
  bash testscript.sh 10 "${compile[$idx]}" "${names[$idx]}"
done