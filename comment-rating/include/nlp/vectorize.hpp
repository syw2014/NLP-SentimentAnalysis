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

        Zeus::nlp::VocabModel* vocabModel_;
        Zeus::nlp::FeatureType featureVec_;
        
    public:
        Vectorize(const std::string& sampleDir, const std::string& dictDir)
          :sampleDir_(sampleDir),dictDir_(dictDir),vocabModel_(NULL)
        {
            vocabModel_ = new Zeus::nlp::VocabModel(dictDir, sampleDir);
        }

        ~Vectorize()
        {
            if(vocabModel_ != NULL)
                delete vocabModel_;
            vocabModel_ = NULL;
        }

        // Select Topk feature as the final features
        void getfeatures(const std::size_t Topk=1000){
            vocabModel_->getFeatures(sampleDir_, featureVec_, Topk);
        }
        
        bool isNeedClean(std::string& str){
            return vocabModel_->isNeedClean(str);    
        }

        void segment(const std::string& line, std::vector<std::string>& token){
            vocabModel_->segment(line, token);
        }
        
        // Construct token as bigram
        void bigram(std::vector<std::string>& token){
            vocabModel_->bigramModel(token);
        }

        // Vectorize for every string
        void vectorize(){
        }

        void store(){
            std::ofstream ofs((sampleDir_+"/feature").c_str());
            for(uint32_t i = 0; i < featureVec_.size(); ++i)
                ofs << featureVec_[i].first << "," << featureVec_[i].second << std::endl;
            ofs.close();
        }

}; 


} // namespace nlp
} // namespace Zeus


#endif // nlp/vectorization.hpp
