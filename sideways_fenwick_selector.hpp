#ifndef SIDEWAYS_FENWICK_RANDOM_SELECTOR
#define SIDEWAYS_FENWICK_RANDOM_SELECTOR
////////////////////////////////IMPORTANT/////////////////////////////////////
//must compile this file using the -std=c++20 flag
//compiling testingDistributions: g++ -std=c++20 test/testingDistributions.cpp

//Note: this was one of many tree based data structures we tested, we made all of the different versions extend a complete tree. 

#include <utility>
#include <vector>
#include <functional>
#include <type_traits>
#include <iostream>
#include <fstream>
#include <limits>
#include <bit>
#include <random>
#include <bitset>

#include "completetree.hpp"





  template <
    typename I = size_t, size_t precision = std::numeric_limits<Real>::digits
  >
  class sideways_fenwick_selector :
    //Extends a complete tree...
    protected complete_tree<I, Real >
  {


    public:

      using size_type = std::ptrdiff_t;
      using index_type = I;
      using This = sideways_fenwick_selector<index_type, precision>;
      /*the tree is indexed beginning at 1 so that moving down the tree requires fewer operations. 
      However, from the users perspective it is zero indexed. We hide the fact that it is indexed 
      at one from the user. index type indicates the index from the users perspective, 
      and node_type indicates the true index. */
      using node_type = index_type; 
      using value_type = Real;
      using entry_type = Real;
      using iterator = value_type*;
      using BaseTree = complete_tree<node_type, value_type>;

      sideways_fenwick_selector() = delete;

      template<typename InputIt>
      sideways_fenwick_selector(InputIt first, InputIt last) :
          BaseTree() 
      {
        size_t n = static_cast<size_t>(last - first);
        InputIt it = first;
        for (index_type i = 0; it != last; ++it, ++i) {
          Real w = *it;
          BaseTree::add_entry(Real(w));
        }
        //go through entire tree and change entries into weightsums of ENTIRE tree
        
        node_type lastNonLeaf = (BaseTree::entry_count())/2;
        if (BaseTree::entry_count()%2==0){
            this->value_of(lastNonLeaf)+=(this->value_of(BaseTree::left_of(lastNonLeaf)));
        }
        else{
            this->value_of(lastNonLeaf)+=(this->value_of(BaseTree::left_of(lastNonLeaf)))+(this->value_of(BaseTree::right_of(lastNonLeaf)));
        }
        for (node_type node = lastNonLeaf-1;node>0;node--){
            this->value_of(node)+=((this->value_of(BaseTree::left_of(node))+(this->value_of(BaseTree::right_of(node)))));
        }
        total_weight = this->value_of(this->root());
        for(node_type node = BaseTree::root();node<lastNonLeaf;node++){
            this->value_of(node)-=(this->value_of(BaseTree::right_of(node)));
        }
        if (BaseTree::entry_count()%2==1){ //bc size in complete tree returns the size including the 0 index
            this->value_of(lastNonLeaf)-=(this->value_of(BaseTree::right_of(lastNonLeaf)));
        }
      }

      sideways_fenwick_selector(sideways_fenwick_selector const&) = default;

      sideways_fenwick_selector(sideways_fenwick_selector &&) = default;

      sideways_fenwick_selector& operator=(sideways_fenwick_selector const&) = default;

      sideways_fenwick_selector& operator=(sideways_fenwick_selector &&) = default;

      ~sideways_fenwick_selector() = default;

      //this is the function to select an element according
      template<class URNG>
      index_type operator()(URNG& g) {
        Real target =  std::generate_canonical<Real, precision, URNG>(g)*total_weight;

        node_type node = this->root();
        node_type lastNonLeaf = BaseTree::entry_count()/2;
        while(node<lastNonLeaf){
          if (target<(this->value_of(node))){
            node = BaseTree::left_of(node);
          }
          else{
            target -=this->value_of(node);
            node=BaseTree::right_of(node);
          }
        }
        //this is to make sure that there is a right child of the last non leaf (if I did this in the loop it would check that there is a right child every time which is unecessary)
        if (node==lastNonLeaf){
          if (target<(this->value_of(node))){
            node = BaseTree::left_of(node);
          }
          else if(BaseTree::right_of(node)<BaseTree::size()){
            target -=this->value_of(node);
            node=BaseTree::right_of(node);
          }
        }
        if (target<(this->value_of(node))){
            return id_of(node);
          }
        else{
          return id_of(nextNode(node));
          
        }


      }

      void update_weight(index_type i, Real new_weight) {
        update_weight_of_node(node_of(i),new_weight);
      }

      Real get_weight(index_type i) {
        return weight_of(node_of(i));
      }


      void push_back(const entry_type& e) {
        value_type v = e;
        BaseTree::add_entry(0);
        update_weight_of_node(BaseTree::last(),v);
      }
      void push_back(const entry_type&& e) {
        value_type v = e;
        BaseTree::add_entry(0);
        update_weight_of_node(BaseTree::last(),v);
      }
      void pop_back() {
        update_weight_of_node(BaseTree::last(),0);
        BaseTree::pop_back();
      }

    private:

      Real total_weight=0;
      //helper function to return the next node to update
      node_type nextNode(node_type currentNode){
        return currentNode>>(((std::countr_one(currentNode)))+1);
      }

      //returns the sum of the left subtree and the node itself
      Real& weightsum_of(node_type n) {
        return this->value_of(n);
      }

      const Real& weightsum_of(node_type n) const {
        return const_cast<This*>(this)->weightsum_of(n);
      }

      

      void update_weight_of_node(node_type givenNode, Real new_weight) {
        auto node = givenNode;
        Real weightDifference =  new_weight - this->weight_of(node);
        total_weight+=weightDifference;
        while(node>=BaseTree::root()){
            this->value_of(node)+=weightDifference; 
            node = nextNode(node); 
        }

      }


      void add_entry(const entry_type& e) {
        value_type v = e;
        BaseTree::add_entry(0);
        update_weight(BaseTree::entry_count(),v);
      }
      void add_entry(const entry_type&& e) {
        value_type v = e;
        BaseTree::add_entry(0);
        update_weight(BaseTree::entry_count(),v);
      }

      Real weight_of(node_type n) {
        auto val = this->value_of(n);
        for (auto i = BaseTree::left_of(n); i <this->size(); i=BaseTree::right_of(i)) {
          val -= this->value_of(i);
          i = BaseTree::right_of(i);
          if (i < this->size()) {
            val -= this->value_of(i);
          }
        }
        return val;
	    }

      const Real& weight_of(node_type n) const {
        return const_cast<This*>(this)->weight_of(n);
      }

      

      void pop_entry() {
        update_weight(BaseTree::last(),0);
        BaseTree::pop_entry();
      }

      sideways_fenwick_selector const& const_this() const {
        return static_cast<This const&>(*this);
      }

      


      //Helper functions to make sure that I don't mess up the offsets (since, from the user's perspectice, id's start at 0, but the nodes are indexed starting at 1)
      index_type id_of(node_type node) {
        return node-1;
      }

      node_type node_of(index_type index){
        return index+1;
      }

      


 };

 #endif