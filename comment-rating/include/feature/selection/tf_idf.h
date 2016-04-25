/*************************************************************************
 @ File Name: tf_idf.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 20 Apr 2016 10:36:21 AM CST
 ************************************************************************/

// TF_IDF model is used to select main features from feature set
// Original Model:
//
//      TF_IDF = TF * IDF (1)
//      IDF = log( D / (DF + 1)) (2)
//      TF = tf / (total term number) (3)
//
//      tf: the number of one term {w} appears in all documnets 
//      DF: the number of documents which contains the term {w}
//      D: the total number of documents
#ifndef FEATURE_SELECTION_TF_IDF_H
#define FEATURE_SELECTION_TF_IDF_H

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace Zeus
{
    
    class TFIDF
    {
        public:

            // Caculate tf_idf when give specific tf, df , docCount
            // tf: term frequency, Type T
            // df: document numbers contain term, Type T
            // docCount: total document number, Type T
            // R: retrun type 
            template<typename T, typename R>
            inline static R caculateTFIDF(const T& tf, const T& df, const T& docCount, const T& termCount){
                // Check errors
                if(tf == 0 || df == 0 || docCount == 0 || termCount == 0){
                    std::cout << "Input parameters to caculate tf_idf was error!\n";
                    return (R)0.0;
                }
                R normTF = (R)tf / (termCount + 1);
                R tf_idf = (R)normTF * log((R)df / (docCount+1));
                return tf_idf;
            }

    };

    // NOTE*: Typename R must be comparabel
    template<typename T, typename R>
    class SORT
    {
        public:
            typedef std::pair<T, R> VectorPair;
            
            // This function will return true if the second value in 'lhs' is greater 
            // than the second in 'rhs'.
           static bool sortDescendBySecond(const VectorPair& lhs, const VectorPair& rhs){
                return lhs.second > rhs.second;
            }
            
            // This function will return true if the second value in 'lhs' is 'less'
            // than the second in 'rhs'.
            static bool sortAscendBySecond(const VectorPair& lhs, const VectorPair& rhs){
                return lhs.second < rhs.second;
            }

    };
 
} // namespace Zeus

#endif // feature/selection/tf_idf.h
