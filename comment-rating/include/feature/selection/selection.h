/*************************************************************************
 @ File Name: selection.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 20 Apr 2016 10:36:21 AM CST
 ************************************************************************/
/*    This headers contains the most useful feature selection model/methods
 * like:
 * TFIDF,CHI-Square, Entropy, etc.
 * */
#ifndef FEATURE_SELECTION_H
#define FEATURE_SELECTION_H

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace Zeus
{
    
    class FeatureSelect
    {
        public:
            
            /* -----------------------TFIDF-----------------------------
             * TFIDF model is used to cacualte term score in nlp.
             * Model description:
             *  TF_IDF = TF * IDF (1)
             *  IDF = log( D / (DF + 1)) (2)
             *  TF = tf / (total term number) (3)
             *  tf: the number of one term {w} appears in all documnets 
             *  DF: the number of documents which contains the term {w}
             *  D: the total number of documents
             *  docCount: total document count
             *  termCount: total term count in vocabulary
               -----------------------TFIDF-----------------------------*/
            template<typename T, typename R>
            inline static R TFIDF(const T& tf, const T& df, const T& docCount, const T& termCount){
                // Check errors
               /* if(tf == 0 || df == 0 || docCount == 0 || termCount == 0){
                    std::cout << "Input parameters to caculate tf_idf was error!\n";
                    return (R)0.0;
                }*/
                R normTF = (R)tf / (termCount + 1);
                R tf_idf = (R)normTF * log((R)df / (docCount+1));
                return tf_idf;
            }
            
            template<typename T, typename R>
            inline static R TFIDF(const T& tf, const T& termCount){
                // Check errors
               /* if(tf == 0 || termCount == 0){
                    std::cout << "Input parameters to caculate tf_idf was error!\n";
                    return (R)0.0;
                }*/
                R normTF = (R)tf / (termCount + 1);
                
                return normTF;
            }
            
            /* -----------------------CHISquare-----------------------------
             *    CHi-Square is a model to caculate the relevancy of text and 
             * one calssified label, and we usually use it to select main features
             * from high feature vector.
             *    Model description:
             *      X^2(t,L1) = N*(A*D - B*C)^2 / (A+C)(A+B)(B+D)(B+C) (1)
             *
             *    Optimization because N,(A+C) and (B+D) are all exact value.
             *      X^2(t,L1) = (A*D - B*C)^2 / (A+B)(C+D) (2)
             *
             *    X^2: test value, N: total document count, M: positive document count
             *    A: document count which contains term t and belongs to label L1,
             *       in fact it's document frequency.
             *    B: document count, contains term t but not belongs to label L1,
             *       in fact it's document frequecny in negative sample set.
             *    C: document count,which don't contain term t but belongs to label L1
             *       when caculation C= M - df(t)
             *    D: document count,not contain term t and not belong to label L1
             *       D = (N-M) - df(t) in negative sample set.
               -----------------------CHISquare----------------------------*/
            template<typename T, typename R>
            inline static R CHiSquare(const T& A, const T& B, const T& C, const T& D){
                R r = (R)(A*D - B*C)*(A*D - B*C) / (A + B + 1)*(C + D + 1);
                return r;
            }
            
    };

    // NOTE*: Typename R must be comparabel
    template<typename T, typename R>
    class SORT
    {
        public:
            typedef std::pair<T, R> VectorPair;
            
            // Return true if the second value in 'lhs' is greater than the second in 'rhs'.
           static bool sortDescendBySecond(const VectorPair& lhs, const VectorPair& rhs){
                return lhs.second > rhs.second;
            }
            
            // Return true if the second value in 'lhs' is 'less' than the second in 'rhs'.
            static bool sortAscendBySecond(const VectorPair& lhs, const VectorPair& rhs){
                return lhs.second < rhs.second;
            }

    };
 
} // namespace Zeus

#endif // feature/selection/selcection.h
