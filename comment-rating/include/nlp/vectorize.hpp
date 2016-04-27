/*************************************************************************
 @ File Name: vectorize.hpp
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Wed 27 Apr 2016 05:04:26 PM CST
 ************************************************************************/
#ifndef NLP_VECTORIZE_H
#define NLP_VECTORIZE_H

#include <iostream>
#include "vocabModel.h"

namespace Zeus
{
namespace nlp
{

class Vectorize
{
    private:
        std::string sampleDir_;
        std::string dictDir_;

        Zeus::nlp::VocabModel* vocab_;
        Zeus::nlp::FeatureType featureVec_;
    
    public:
        Vectorize(const std::string& sampleDir, const std::string& dictDir)
          sampleDir_(sampleDir),dictDir_(dictDir),vocab_(NULL)
        {
        }

        ~Vectorize()
        {
            if(vocab_ != NULL)
                delete vocab_;
            vocab_ = NULL;
        }

        void getfeatures(){
        }
        
        void segment(){
        }

        void bigram(){
        }

        void vectorize(){
        }

        void store(){
        }



}; 


} // namespace nlp
} // namespace Zeus


#endif // nlp/vectorization.hpp
