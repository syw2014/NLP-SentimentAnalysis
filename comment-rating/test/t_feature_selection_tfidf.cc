/*************************************************************************
 @ File Name: t_feature_selection_tfidf.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 20 Apr 2016 01:19:02 PM CST
 ************************************************************************/
#include <iostream>
#include "feature/selection/tf_idf.h"

int main(int argc, char* argv[])
{
   float r = Zeus::TFIDF::caculateTFIDF<int, float>(1,1,1,1);
   std::cout << "TF_IDF: " << r << std::endl;

   std::cout << "Test : Ascending sort \n";
   
   std::vector<Zeus::SORT<std::string, int>::VectorPair> vec;
   vec.push_back(std::make_pair("a", 8));
   vec.push_back(std::make_pair("a", 9));
   vec.push_back(std::make_pair("a", 0));
   vec.push_back(std::make_pair("a", 2));
   vec.push_back(std::make_pair("a", 3));
   vec.push_back(std::make_pair("a", 1));
   vec.push_back(std::make_pair("a", 5));

   std::cout << "\tBefore sorting: \n";
   for(int i = 0; i < vec.size(); ++i){
       std::cout <<"\t" <<vec[i].first << "," << vec[i].second << std::endl;
   }
   
   std::cout << "---------------------------\n \
            \n\tDescending sort:\n";
   std::sort(vec.begin(), vec.end(), Zeus::SORT<std::string, int>::sortDescendBySecond );
   for(int i = 0; i < vec.size(); ++i){
       std::cout <<"\t" <<vec[i].first << "," << vec[i].second << std::endl;
   }
   
   std::sort(vec.begin(), vec.end(), Zeus::SORT<std::string, int>::sortAscendBySecond );
   std::cout << "---------------------------\n \
            \n\tAscending sort:\n";
   for(int i = 0; i < vec.size(); ++i){
       std::cout <<"\t" <<vec[i].first << "," << vec[i].second << std::endl;
   }
}

