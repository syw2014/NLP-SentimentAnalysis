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

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iomanip>

#include "linear/linear.h"
#include "common/common_header.h"


namespace ml
{

// linear model sample data type pointer, which contains (int)index and (double)value,
typedef struct feature_node FeatureType;

// Sample predict,load the trained model to predict class, sample usage:
//      double label = SamplePredict(sample x);
//      BatchPredict(Sample-file-name);
//      

class Predict
{
    private:
        //int flag_predict_probability_; //  a flag to print predict probability for logistic regression
        int correct_;         // the number of correct prediction sample
        int total_;           // the total number of predict sample
        double error_;        // the mean square error
        double predict_sum_;  // predict label summation
        double target_sum_;   // target label summation
        double predict_square_sum_; // all the predict label square summation 
        double target_square_sum_;  // all the target label square summation
        double predict_target_sum_; // all the target label square summation

        int nr_class_, nr_feature_, n_; // class number, feature dimension, to determine whether there's a bias

        struct model* model_;           // linear model


    public:
        Predict(const std::string& model)
        : correct_(0)
          ,total_(0)
          ,error_(0)
          ,predict_sum_(0)
          ,target_sum_(0)
          ,predict_square_sum_(0)
          ,target_square_sum_(0)
          ,predict_target_sum_(0)
          ,nr_class_(0)
          ,nr_feature_(0) ,n_(0){
              
              if(model.empty())
                  return;
              // load linear model
              model_ = load_model(model.c_str());
              if(model_ == 0){
                  std::cout << "Load model failed!\n";
                  return;
              }

              nr_class_ = get_nr_class(model_);
              nr_feature_ = get_nr_feature(model_);
              std::cout << "Model: nr_class:  " << nr_class_ << std::endl;
              std::cout << "Model: nr_feature:  " << nr_feature_ << std::endl;

              if(model_->bias >= 0)
                  n_ = nr_feature_ + 1;
              else
                  n_ = nr_feature_; 
        }

        ~Predict(){
            free_and_destroy_model(&model_);
        }
        
        // GET number of class
        int GetNRClass(){
            return nr_class_;
        }
        // GET number of feature
        int GetNRFeature(){
            return nr_feature_;
        }
        // GET bias if exist 
        double GetModelBias(){
            return model_->bias;
        }

        // Interface:
        // Returns the predicted class label for input sample, in this case is +1 or -1. 
        // Make sure the sample type is 'struct feature_node*'.
        double SamplePredict(const FeatureType* sample){
           return predict(model_, sample); 
        }
        
        // Interface:
        // Returns the predicted class label and the probability belongs to each class.
        // The output probability likes: 0.5000(lable +1) 0.5000(label -1)
        double PredictWithProbability(const FeatureType* sample, double* prob_estimates){
            return predict_probability(model_, sample, prob_estimates);
        }

        // Interface:
        // Batch predict, loading sample from file, the struct of every sample in file is
        // 'label1 idx1:val1 idx2:val2 ...', after process this method will output the predict
        // accuracy ,correct predict number and total sample number.
        void BatchPredict(const std::string& filename){
            if(filename.empty()){
                std::cout << "File \"" << filename << "\" is not exist!\n";
                return;
            }
            std::ifstream ifs(filename.c_str());
            if(!ifs.is_open()){
                std::cout << "File \"" << filename << "\" open failed!\n";
                return;
            }
            std::ofstream ofs("sample.pre");
            std::string line;
            double target_label, predict_label;
            double* prob_estimates;
            prob_estimates = (double*)malloc(nr_class_*sizeof(double));
            int max_nr_attr = 64; // maximum number of value
            FeatureType* x; // feature pointer
            x = (FeatureType*)realloc(x, max_nr_attr*sizeof(FeatureType));
            while(getline(ifs, line)){
                if(line.empty())
                    continue;
                std::vector<std::string> vec;
                // vec[0]: label, vec[1...i]: index:value
                boost::split(vec, line, boost::is_any_of(" "));
                if(vec.size() < 2)
                    continue;
                try{
                    target_label = boost::lexical_cast<double>(vec[0]);
                } catch (...){
                    vec.clear();
                    continue;
                }
                // parse every index:value pair and store in sample x
                int j=0,i=0; // j is loop control variable, i is the index of 'x'
                for(j=1; j < vec.size(); ++j){
                    if(i >= max_nr_attr - 2){ // need one more for index = -1
                        max_nr_attr *= 2;
                        x = (struct feature_node*) realloc(x, max_nr_attr*sizeof(struct feature_node));
                    }
                        std::string::size_type pos = vec[j].find(':');
                        if(pos == std::string::npos)
                            continue;
                        int idx;
                        double val;
                        try{  // if index and value parse failed then abandon
                            idx = boost::lexical_cast<int>(vec[j].substr(0, pos));
                            val = boost::lexical_cast<double>(vec[j].substr(pos+1));
                        }catch(...){
                            continue;
                        }

                        x[i].index = idx;
                        x[i].value = val;
                        // feature indices larger than those in training are not used
                        if(x[i].index <= nr_feature_)
                            ++i;
                        else
                            break;
                }

                // if the bais is exist, then add a element in sample struct pointer
                // index = nr_feature(default) +1
                // value = model_bias;
                if( model_->bias >= 0){
                    x[i].index = n_;
                    x[i].value = model_->bias;
                    i++;
                }
                // indicate this is the end of feature struct pointer
                x[i].index = -1; 
                
                // predict
               predict_label = PredictWithProbability(x, prob_estimates);
               ofs << predict_label;
               for(int k = 0; k < nr_class_; ++k)
                    ofs <<" "<< std::setprecision(6) << prob_estimates[k];
               ofs << std::endl;
               if(predict_label == target_label)
                    ++correct_;
                // Accumulation errors
                error_ += (predict_label - target_label)*(predict_label - target_label);
                predict_sum_ += predict_label;
                target_sum_ += target_label;
                predict_square_sum_ += predict_label * predict_label;
                target_square_sum_ += target_label * target_label;
                predict_target_sum_ += predict_label * target_label;
                ++total_;
            }
            ifs.close();
            ofs.close();
            // Print the final result
/*            std::cout << "Mean squared error = " << (double)error_ / total_ << std::endl;
            std::cout << "Squared correlation coefficient = " 
                << (double)((total_*predict_target_sum_ - predict_sum_*target_sum_)
                            *(total_*predict_target_sum_ - predict_sum_*target_sum_)) / 
                            ((total_*predict_square_sum_ - predict_sum_*predict_sum_)
                             *(total_*target_square_sum_ - target_sum_*target_sum_)) << std::endl;*/
  
            std::cout << "Accuracy = " << (double)correct_ / total_*100 << "% (" << correct_  << "/" << total_ << ")\n";
            free(x);
            free(prob_estimates);
        }

};
} // namespace ml

#endif // ml/predict.hpp

