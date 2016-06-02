/*************************************************************************
 @ File Name: rating.hpp
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Tue 31 May 2016 04:02:37 PM CST
 ************************************************************************/
#ifndef ML_RATING_HPP
#define ML_RATING_HPP

#include "nlp/vectorize.hpp"
#include "ml/predict.hpp"

/*
 * Comment rating module has been wrapped based on sample vectorize module and
 * Liblinear(a branch of support vector machine) model. Firstly loading sample
 * feature vector and trained model, then generating vector for given sample,use
 * this vector to predict sample label, the accuracy rate is about 85.6% not to
 * to be optimizated.
 */

namespace ml
{
    
// Comment rating, when giving a arbitrary text then output the commet rating
// Usage:
//      ml::Rating* pRate = NULL;
//      pRate = new ml::Rating(featureDir, dictDir, modelDir);
//      std::string text(""); // arbitrary context
//      pRate->PredictClassLabel(text);
//
class Rating
{
    private:
        std::string feature_dir_; // text feature path
        std::string dict_dir_;    // segment dictionary path
        std::string model_dir_;   // classification model

        int nr_class_, nr_feature_; // label number and feature number
        double bias_; // model bias


        Zeus::nlp::Vectorize* pVectorize_; 
        ml::Predict* pPredict_;   // predict with training model

    public:
        Rating(const std::string& featureDir, const std::string& dictDir, const std::string& modelDir)
          :feature_dir_(featureDir)
           ,dict_dir_(dictDir)
           ,model_dir_(modelDir)
           ,nr_class_(0)
           ,nr_feature_(0)
           ,bias_(0.0)
           ,pVectorize_(NULL)
           ,pPredict_(NULL){
               if(feature_dir_.empty() || dict_dir_.empty() || model_dir_.empty())
                   return;

                pVectorize_ = new Zeus::nlp::Vectorize(feature_dir_, dict_dir_);
                pPredict_ = new ml::Predict(model_dir_);

                pVectorize_->loadFeature();

                // get model attributes
                nr_class_ = pPredict_->GetNRClass();
                nr_feature_ = pPredict_->GetNRFeature();
                bias_ = pPredict_->GetModelBias();
           }
        
        ~Rating(){
            if(pVectorize_ != NULL){
                delete pVectorize_;
                pVectorize_ = NULL;
            }
            if(pPredict_ != NULL){
                delete pPredict_;
                pPredict_ = NULL;
            }
        }

        // Returns the vector of one text string
        void GenerateSampleVector(const std::string& str, std::vector<float>& element){
            pVectorize_->process(str, element);
        }
        
        // Convert a vector of one text to model sample type, which is a struct
        // '{int idx; double value}'.
        void VectorToSampleType(const std::vector<float>& element, ml::FeatureType* pSample){
            if(element.empty()){
                pSample = NULL;
                return;
            }
            
            int max_size = 64; // default memory size of pSample
            int i, j=0;
            for(i = 0; i < element.size(); ++i){
                if(element[i] == 0)
                    continue;
               // Realloc memory for pSample
               if(j > max_size - 2){
                   max_size *= 2;
                   pSample = (ml::FeatureType*)realloc(pSample, max_size*sizeof(ml::FeatureType));
               }
               pSample[j].index = int(i+1);
               pSample[j].value = element[i];
               // Debug info
               std::cout << i+1 << ":" << element[i] << ",";
               if(j <= nr_feature_)
                   ++j;
               else
                   break;
            }
            // Debug info
            if(bias_ >= 0){
                pSample[j].index = nr_feature_+1;
                pSample[j].value = bias_;
                j++;
            }
            // Terminator for sample
            pSample[j].index = int(-1);
        }
        
        // Predict label and output it's
        double PredictClassLabel(const std::string& str,double& predict_prob){
            if(str.empty())
                return double(0.0);
            std::vector<float> element;
            ml::FeatureType* pSample;
            int max_size = 64;
            pSample = (ml::FeatureType*)malloc(max_size*sizeof(ml::FeatureType));
            GenerateSampleVector(str, element);
            VectorToSampleType(element, pSample);
            PredictWithProbability(pSample, predict_prob);
            
            free(pSample);
        }


        // Predict which label(+1 or -1) the text belongs to.
        void PredictWithProbability(const ml::FeatureType* pSample, double& predict_prob){
            double* prob_estimiates = NULL;
            prob_estimiates = (double*)malloc(nr_class_*sizeof(double));
            prob_estimiates[0] = 1.1;
            double label = pPredict_->PredictWithProbability(pSample, prob_estimiates);
            std::cout << "[INFO] Predict Result: \"label\": " << label 
                << "\" prob\":";
            for(uint32_t i = 0; i < nr_class_; ++i)
                std::cout <<" "<<std::setprecision(6) << prob_estimiates[i];
            std::cout << std::endl;
            predict_prob = prob_estimiates[0];
            free(prob_estimiates);
        }
};

} // namesapce ml
#endif // ml/rating.hpp

