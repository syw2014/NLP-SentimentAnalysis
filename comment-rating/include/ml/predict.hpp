/*************************************************************************
 @ File Name: predict.hpp
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 27 May 2016 04:05:47 PM CST
 ************************************************************************/
// A method to packaging predict methods of (liblinear or libsvm) with c++
// when to use predict the results when given a sample, firstly it  will 
// load the trained model, and call vectorized method then send the 
// vector to predict model.

#ifndef ML_PREDICT_HPP
#define ML_PREDICT_HPP

#include "svm/svm.h"
#include "linear/linear.h"

namespace ml
{
class Predict
{
    private:
        int correct_num_; // the number of correct prediction sample
        int total_num_;   // the total number of predict sample
        double error_;    // the mean square error
        double predict_sum_;  // predict label summation
        double target_sum_;   // target label summation
        double predict_square_sum_; // all the predict label square summation 
        double target_square_sum_;  // all the target label square summation

        struct model* model_;       // linear model


    public:
        Predict()
        : correct_num_(0)
          ,total_num_(0)
          ,error_(0)
          ,predict_sum_(0)
          ,target_sum_(0)
          ,predict_square_sum_(0)
          ,target_square_sum_(0) {
        }

        // Interface
        // Predict a sample label
        void predict(){
        }

};
} // namespace ml



