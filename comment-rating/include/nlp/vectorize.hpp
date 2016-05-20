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


typedef boost::unordered_map<std::string, uint32_t> TermTFType;
typedef std::pair<std::size_t, int> IdLabelType;
typedef std::pair<IdLabelType, std::vector<float> > SampleVectorType;

class Vectorize
{
    private:
        std::string sampleDir_;
        std::string dictDir_;

        Zeus::nlp::VocabModel* vocabModel_;
        Zeus::nlp::FeatureType featureVec_;
        std::vector<SampleVectorType> sampleVec_;    
        std::size_t sampleID_; 

        void vectorize_(const TermTFType& termTF, std::vector<float>& weight){
            weight.clear();
            std::size_t feat_size = featureVec_.size();
            if(feat_size == 0){
                std::cerr << "Vectorize failed, the feature vector size is zero!!!\n";
                return;
            }
            weight.resize(feat_size);
            float sum = 0.01;
            TermTFType::const_iterator it;
            for(std::size_t i = 0; i < feat_size; ++i){
                it = termTF.find(featureVec_[i].first);
                uint32_t tf = 1;
                if(it != termTF.end()){
                    tf = it->second;
                float w = (float)tf * featureVec_[i].second;
                weight[i] = w;
                //std::cout << "TEST - term: " << featureVec_[i].first << ":" << featureVec_[i].second << ":" << tf << std::endl;
                sum += w*w;
                }else{
                    weight[i] = 0.0;
                }
            }
            sum =(float)sqrt(sum);
            // normalize
            for(uint32_t j = 0; j < feat_size; ++j)
                weight[j] /= sum;
        }

    public:
        Vectorize(const std::string& sampleDir, const std::string& dictDir)
          :sampleDir_(sampleDir),dictDir_(dictDir),sampleID_(0),vocabModel_(NULL)
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
            clock_t start, end;
            start = clock();
            vocabModel_->getFeatures(sampleDir_, featureVec_, Topk);
            end = clock();
            std::cout << "[LOG INFO] Feature creation completed , time: "
                    << (double)(end - start)/CLOCKS_PER_SEC/60 << "min." << std::endl;
        }
       
        // Parse key value
        bool parseKeyValue(const std::string& line
                            ,uint16_t delimiter
                            ,std::string& key
                            ,std::string& value){
            if(line.empty() || delimiter == 0)
                return false;
            std::string::size_type pos;
            pos = line.find(delimiter);
            if(pos == std::string::npos)
                return false;
            key = line.substr(0, pos);
            value = line.substr(pos+1);
            return true;
        }

        // Load features from feature
        bool loadFeature(){
            std::ifstream ifs((sampleDir_+"/feature").c_str());
            if(!ifs.is_open()){
                std::cout << "Load feature file failed!\n";
                return false;
            }
            std::string line;
            while(getline(ifs, line)){
                std::string key,value;
                if(!parseKeyValue(line, ',', key, value))
                    continue;
                float w = 0.0;
                try{
                    w = boost::lexical_cast<float>(value);
                }catch(...){
                    w = 0.0;
                }
                featureVec_.push_back(std::make_pair(key, w));
            }
            ifs.close();
            std::cout << "Features loaded size : " << featureVec_.size() << std::endl;
            return true;
        }

        // clean special chars and stop words
        bool isNeedClean(std::string& str){
            return vocabModel_->isNeedClean(str);    
        }
        
        void clean(std::vector<std::string>& token){
            std::vector<std::string>::iterator it;
            for(it = token.begin(); it != token.end(); ++it){
                if(isNeedClean(*it) || (*it).empty()){
                    token.erase(it);
                    it--;
                }
            }
        }

        void segment(const std::string& line, std::vector<std::string>& token){
            vocabModel_->segment(line, token,false);
        }
        
        // Construct token as bigram
        void bigram(std::vector<std::string>& token){
            vocabModel_->bigramModel(token);
        }

        // term process contains deduplication, tf calculation
        void termProcess(std::vector<std::string>& token, TermTFType& termTF){
            clean(token);
            bigram(token);
            termTF.clear();
            TermTFType::iterator it;
            for(std::size_t i = 0; i < token.size(); ++i){
                it = termTF.find(token[i]);
                if(it != termTF.end())
                    it->second += 1;
                else
                    termTF[token[i]] = 1;
            }
        }
        
        // Process , string segment , term process, and return it's vector
        void process(const std::string& line, std::vector<float>& element){
            std::vector<std::string> token;
            element.clear();
            TermTFType termTF;
            segment(line, token);
            termProcess(token, termTF);
            vectorize_(termTF, element);
        }
        
        // To check all the element is not zero,
        bool isAllZero(const std::vector<float> element){
            if(element.empty())
                return true;
            for(uint32_t i = 0; i < element.size(); ++i)
                if(element[i] > 0.0)
                    return false;
            return true;
        }
        // Main process 
        bool readLine(const std::string& line, std::ofstream& ofs){
            std::vector<std::string> token;
            std::vector<float> element;
            std::string::size_type pos = line.find('\t');
            if(pos == std::string::npos)
                return false;
            std::string label = line.substr(0, pos);
            std::string content = line.substr(pos+1);
            IdLabelType id_label;
            int L;
            if(label == "好评")
                L = 1;
            else if(label == "差评")
                L = -1;
            id_label = std::make_pair(sampleID_, L);
            element.clear();
            // vectorize
            process(content, element);
            if(isAllZero(element))
                return false;
            // Flush sample ID, label
            ofs << sampleID_ << "," << L;
           // for(uint32_t i = 0; i < element.size(); ++i)
           //     ofs << "," << element[i];
            // TODO Optimization
            // Data compression, we only store the element which score is bigger than zero
            // the store formate like : 'index:score'
            std::stringstream ss;
            std::string score;
            for(uint32_t i = 0; i < element.size(); ++i){
                if(element[i] <= 0.0)
                    continue;
                ss << i << ":" << element[i];
                score = ss.str();
                ofs << "," << score;
                ss.str("");
            }
            ofs << std::endl;
            //sampleVec_.push_back(std::make_pair(id_label, element));
            sampleID_ += 1;
            return true;
        }

        // Load samples and vectorize every sample
        void vectorize(){
            // positive
            // use samples.pos_50 to test the whole process.
            std::ifstream ifs((sampleDir_+"/samples.pos").c_str());
            if(!ifs.is_open()){
                std::cout << "Open samples error!\n";
                return;
            }
            std::ofstream ofs((sampleDir_+"/vectorization").c_str());
            std::string line;
            clock_t start, end;
            start = clock();
            std::cout << "[LOG INFO] Start to vectorize positive samples...\n";
            while(getline(ifs, line)){
                if(!readLine(line, ofs))
                    continue;
            }
            end = clock();
            ifs.clear();
            ifs.close();
           // ofs.close();
            std::cout << "[LOG INFO] Positive samples vectorization completed! Time cost: "
                    << (double)(end - start)/CLOCKS_PER_SEC/60 << "min." 
                    <<" Positive size: " << sampleVec_.size() << std::endl;

            ifs.open((sampleDir_+"/samples.neg").c_str());
            std::cout << "[LOG INFO] Start to vectorize positive samples...\n";
            start = clock();
            while(getline(ifs, line)){
                if(readLine(line, ofs))
                    continue;
            }
            end = clock();
            ifs.clear();
            ifs.close();
            ofs.close();
            std::cout << "[LOG INFO] Negative samples vectorization completed! Time cost: "
                    << (double)(end - start)/CLOCKS_PER_SEC/60 << "min."
                    <<" Negative size: " << sampleVec_.size() << std::endl;
        }

        void store(){
           /* std::ofstream ofs((sampleDir_+"/feature").c_str());
            for(uint32_t i = 0; i < featureVec_.size(); ++i)
                ofs << featureVec_[i].first << "," << featureVec_[i].second << std::endl;
            ofs.clear();
            ofs.close();*/
            std::ofstream ofs((sampleDir_+"/vectorization").c_str());
            for(std::size_t j = 0; j < sampleVec_.size(); ++j){
                std::size_t size = sampleVec_[j].second.size();
                ofs << sampleVec_[j].first.first << "," << sampleVec_[j].first.second;
                for(std::size_t k = 0; k < size; ++k)
                    ofs << "," << sampleVec_[j].second[k];
                ofs << std::endl;
            }
            ofs.close();
        }
}; 


} // namespace nlp
} // namespace Zeus


#endif // nlp/vectorization.hpp
