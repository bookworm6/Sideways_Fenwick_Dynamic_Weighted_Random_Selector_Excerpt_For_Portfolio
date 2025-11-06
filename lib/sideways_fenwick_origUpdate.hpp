#ifndef SIDEWAYS_FENWICK_RANDOM_SELECTOR_ORIG_UPDATE
#define SIDEWAYS_FENWICK_RANDOM_SELECTOR_ORIG_UPDATE
////////////////////////////////IMPORTANT/////////////////////////////////////
//compile this file using the -std=c++20 flag. For best performance results, include the -O3 flag. 
//compiling testingDistributions: g++ -std=c++20 -O3 test/testingDistributions.cpp


/*Note: This library is meant to replace the current std::discrete_distribution, so it needs to fulfill the 
random number distribution requirements https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution.html.
This led to design decisions such as the existence of the class Param and the storage of the tree within Param that 
I would not have made otherwise. Param and the sideways_fenwick_selector_origUpdate are tightly bound, and seperate only 
for comptatability with the standard library's random_number_distribution. Ideally they would be a single class, so 
we made sideways_fenwick_selector_origUpdate a friend of Param to give it the access it requires. */


#include <utility>
#include <vector>
#include <functional>
#include <type_traits>
#include <iostream>
#include <fstream>
#include <limits>
#include <bit>
#include <bitset>
#include <random>



  template <
    typename result_type = size_t, size_t precision = std::numeric_limits<double>::digits 
  >
  class sideways_fenwick_selector_origUpdate 
  {


    public:

      /*Note: there are two indexing systems: From the user's perspective, all of the nodes are indexed starting at 0. 
      However, in reality having nodes indexed starting at 1 allows us to move right down a binary tree using a single 
      bitshift operaton. Indexes are seperated into index_type (from the user's perspective) and node_type for code 
      clarity and ease of debugging.*/ 
      using index_type = result_type;
      using node_type = result_type; 


      class param_type_orig{
        public:
          template<typename InputIt>

          //constructing the underlying tree
          param_type_orig(InputIt first, InputIt last){
            _tree.push_back(0); //first element of the tree aray will contain the total weight (the tree is indexed starting at 1)

            //initializing tree to contain the weights inputed
            for (InputIt it=first; it != last; ++it) {
              _tree.push_back(double(*it));
            }

            //go through entire tree and change entries into weightsums of ENTIRE tree
            node_type lastNonLeaf = entry_count()/2;
            if (entry_count()%2==0){
              _tree[lastNonLeaf]+=_tree[left_of(lastNonLeaf)];
            }
            else{
              _tree[lastNonLeaf]+=(_tree[left_of(lastNonLeaf)]+_tree[right_of(lastNonLeaf)]);
                
            }
            for (node_type node = lastNonLeaf-1;node>0;node--){
              _tree[node]+=(_tree[left_of(node)]+_tree[right_of(node)]);
            }

            _tree[0]=_tree[root()]; //making _tree[0] contain the total weight of the tree, which is currently in the root

            //go through entire tree again (this time from the top) and subtract the weight of the right subtree
            for(node_type node = root(); node<lastNonLeaf; node++){
                _tree[node]-=_tree[right_of(node)];
            }
            if (entry_count()%2==1){ 
              _tree[lastNonLeaf]-=_tree[right_of(lastNonLeaf)];
            }

          }
          param_type_orig(param_type_orig const&) = default; //copy constructor

          param_type_orig(param_type_orig &&) = default; //move constructor

          //default distribution (singular entry with a weight of one - the total weight is one - it will always select entry 0)
          param_type_orig(){
            _tree.push_back(1);
            _tree.push_back(1);
          }

          param_type_orig& operator=(param_type_orig const&) = default; //assignment operator

          param_type_orig& operator=(param_type_orig &&) = default; //move assignment operator

          ~param_type_orig() = default;

          bool operator==(const param_type_orig& other )const{
            if (_tree!=other._tree){
              return false;
            }
            return true;

          }

          bool operator!=(const param_type_orig& other)const{
            return !(operator==(other));
          }

          result_type min()const{
            return 0;
          }
          result_type max()const{
            return _tree.size()-2;
          }

          //This exists for debugging and demonstration purposes
          void printTree()const{ 
            std::cout<<"tree stored in _tree: ";
            for(int i=1;i<_tree.size();i++){
              std::cout<<_tree[i]<<", ";
            }
            std::cout<<std::endl;
          }
          void update_weight(index_type i, double new_weight) {
            update_weight_of_node(node_of(i),new_weight);
          }

          double get_weight(index_type i) const{
            return weight_of(node_of(i));
          }

          void push_back(const double& e) {
            _tree.push_back(0);
            update_weight_of_node(_tree.size()-1,e);
          }
          void push_back(const double&& e) {
            _tree.push_back(0);
            update_weight_of_node(_tree.size()-1,e);
          }
          void pop_back() {
            update_weight_of_node(_tree.size()-1,0);
            _tree.pop_back();
          }

          friend class sideways_fenwick_selector_origUpdate<result_type,precision>;


        private:
          std::vector<double> _tree;



          void update_weight_of_node(node_type givenNode, double new_weight) {
            auto node = givenNode;
            double weightDifference =  new_weight - weight_of(node);
            _tree[0]+=weightDifference;

            while(node>=root()){
                _tree[node]+=weightDifference; 
                node = nextNode(node); 
            }

          }

          
          double weight_of(node_type n) const{
            double val = _tree[n];
            for (auto i = left_of(n); i <_tree.size(); i=right_of(i)) {
              val -= _tree[i];
            }
            return val;
          }
          size_t entry_count()const{
            return _tree.size()-1;
          }

      };

      template<typename InputIt>
      sideways_fenwick_selector_origUpdate(InputIt first, InputIt last):
        parameters(first,last)
      {
        
      }
      sideways_fenwick_selector_origUpdate():
        parameters()
      {

      }

      sideways_fenwick_selector_origUpdate(sideways_fenwick_selector_origUpdate const&) = default; //copy constructor

      sideways_fenwick_selector_origUpdate(sideways_fenwick_selector_origUpdate &&) = default; //move constructor

      sideways_fenwick_selector_origUpdate& operator=(sideways_fenwick_selector_origUpdate const&) = default; //assignment operator

      sideways_fenwick_selector_origUpdate& operator=(sideways_fenwick_selector_origUpdate &&) = default; //move assignment operator

      ~sideways_fenwick_selector_origUpdate() = default;

      friend std::ostream& operator<<(std::ostream& stream,const sideways_fenwick_selector_origUpdate<result_type,precision>& selector){
        auto params=selector.param();
        stream<<"underlyingVector ";
        for(int i=0;i<params._tree.size();i++){
          stream<<params._tree[i]<<" ";
        }
        stream<<"END"<<std::endl; //go through entire tree and change entries into weightsums of ENTIRE tree
        return stream;
      }

      friend std::istream& operator>>(std::istream& stream,sideways_fenwick_selector_origUpdate<result_type,precision>& selector){
        std::string lastReadIn;
        std::vector<double> results;
        stream>>lastReadIn;
        if (lastReadIn!="underlyingVector"){
          stream.setstate(std::ios::failbit);
          return stream;
        }
        stream>>lastReadIn;
        for(int i=0;lastReadIn!="END";i++,stream>>lastReadIn){
          try{
            results.push_back(std::stod(lastReadIn));
          }
          catch(std::invalid_argument){
            stream.setstate(std::ios::failbit);

          }
        }
        selector.parameters._tree=results;
        return stream;

      }

      std::vector<double> probabilities()const{
        double totalWeight = parameters._tree[0];
        std::vector<double> probs = get_all_weights();
        for(int i=0;i<probs.size();i++){
          probs[i]=probs[i]/totalWeight;
        }
        return probs;

      }

      std::vector<double> get_all_weights()const{
        //copying tree into weights without copying total weight 
        std::vector<double> weights = parameters._tree; //copies the tree into weights 
        node_type lastNonLeaf=parameters.entry_count()/2;
        if(parameters.entry_count()%2==1){
          weights[lastNonLeaf]+=weights[right_of(lastNonLeaf)];

        }
        for(node_type node = lastNonLeaf-1;node>0;node--){
          weights[node]+=weights[right_of(node)];
        }
        for(node_type node=root();node<lastNonLeaf;node++){
          weights[node]=weights[node]-weights[right_of(node)]-weights[left_of(node)];
        }
        if (parameters.entry_count()%2==1){
          weights[lastNonLeaf]=weights[lastNonLeaf]-weights[right_of(lastNonLeaf)]-weights[left_of(lastNonLeaf)];
        }
        else{
          weights[lastNonLeaf]-=weights[left_of(lastNonLeaf)];
        }
        weights.erase(weights.begin());
        return weights;
      }

      
      template<class URNG>
      index_type operator()(URNG& g)const{
        return operator()(g,parameters);

      }

      //This is the function to select an element based on its weight. 
      template<class URNG>
      index_type operator()(URNG& g,const param_type_orig& inputParams)const{
        std::vector<double>tree=inputParams._tree;
        double target =  std::generate_canonical<double, precision, URNG>(g)*tree[0]; //_tree[0] stores total weight

        node_type node = root();
        node_type lastNonLeaf = (tree.size()-1)/2;
        
        while(node<lastNonLeaf){
          if (target<(tree[node])){
            node = left_of(node);
          }
          else{
            target -=tree[node];
            node=right_of(node);
          }
        }

        if (node==lastNonLeaf){
          if (target<(tree[node])){
            node = left_of(node);
          }
          else if(right_of(node)<tree.size()){
            target -=tree[node];
            node=right_of(node);
          }
        }

        if (target<(tree[node])){
            return id_of(node);
          }
        else{
          return id_of(nextNode(node));
          
        }


      }

      bool operator==(const sideways_fenwick_selector_origUpdate& other )const{
        if (parameters==other.params()){
          return true;
        }
        return false;

      }

      bool operator!=(const sideways_fenwick_selector_origUpdate& other)const{
        return !(operator==(other));
      }



      void update_weight(index_type i, double new_weight) {
        parameters.update_weight(i,new_weight);
      }

      double get_weight(index_type i) const{
        return parameters.get_weight(i);
      }

      void push_back(const double& e) {
        parameters.push_back(e);
      }
      void push_back(const double&& e) {
        parameters.push_back(e);
      }
      void pop_back() {
        parameters.pop_back();
      }

      result_type min()const{
        return parameters.min();
      }
      result_type max()const{
        return parameters.max();
      }

      void reset() {}

      param_type_orig param()const{
        return parameters;
      }
      void param(const param_type_orig& newParam){
        parameters = newParam;
      }





    private:
      
      //helper function to return the next node to update
      static node_type nextNode(node_type currentNode){
        return currentNode>>(((std::countr_one(currentNode)))+1);
      }

    //Helper functions to make sure that I don't mess up the offsets 
      static index_type id_of(node_type node) {
        return node-1;
      }

      static node_type node_of(index_type index){
        return index+1;
      }


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //tree methods and data
      param_type_orig parameters;

      static constexpr node_type root(){ return 1; }

      static node_type parent_of(node_type node) {
        return ((node) >> 1);
      }

      static node_type left_of(node_type node) {
        return (node << 1);
      }

      static node_type right_of(node_type node) {
        return (node << 1) + 1;
      }

 };

 #endif