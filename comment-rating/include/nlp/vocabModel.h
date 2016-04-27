/*************************************************************************
 @ File Name: vocabModel.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 15 Apr 2016 10:49:33 AM CST
 ************************************************************************/
/* 
 * Vocabulary model to generate avaiable features from given samples for
 * text classification, and in this module we used both feature selection
 * (TF-IDF,CHI-Square,Entropy) and extraction methods(Topic modeling, PLSA,
 * LDA), we given all the implementations of them but we only chose one to
 * generate features, and the default is TF-IDF you can find  other methods
 * in 'include/feature/', you can try it 'featureSelection()', but in the 
 * first version the performance may be need optimizated,wish you happy to
 * use it.
 *
*/


#ifndef ZEUS_NLP_VOCABMODEL_H
#define ZEUS_NLP_VOCABMODEL_H

#include <assert.h>
#include <set>
#include <iterator>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "segmentWrapper.h"
#include "feature/selection/selection.h"


namespace Zeus
{
namespace nlp
{

typedef struct
{
    size_t A; // parameter as the same in chi-square
    size_t B;
    size_t tf; // global term frequency
    float score; 
}TermInfoType;



// Golbel Vocabulary type
typedef boost::unordered_map<std::string, TermInfoType> VocabType;

// Golbel Feature type
// Structure: term, score/weight
typedef std::vector<Zeus::SORT<std::string, float>::VectorPair > FeatureType;

enum{negitive=0,positive};
#define posLabel "好评"
#define negLabel "差评"

//#define DEBUG

class VocabModel
{
    private:

        std::string dictDir_;
        std::string sampleDir_;
        
        // total document number
        size_t posDocCnt_;
        size_t negDocCnt_;

        // Global term vocabulary
        VocabType vocab_;
        FeatureType featureVec_; // selected features
        boost::unordered_map<std::string, float> termScore_;
        // Stop words
        boost::unordered_set<std::string> stopwords_;

        // Normalization, to lower case, remove white space at head and tail 
        std::string normalize_(const std::string& line){
            std::string nline(line);

            boost::algorithm::to_lower(nline);
            boost::algorithm::trim(nline);

            return nline;
        }

        // Remove stop words from token
        void removeStopWords_(std::vector<std::string>& vec){
            std::vector<std::string>::iterator it;
            for(it = vec.begin(); it != vec.end(); ++it){
                if(stopwords_.end() != stopwords_.find(*it)){
                    vec.erase(it);
                    it -= 1;
                }
            }
        }
        
        bool loadStopWords_(const std::string& filename){
            if(filename.empty())
                return false;
            std::ifstream ifs(filename.c_str());
            std::string line;
            while(getline(ifs, line)){
                if(line.empty())
                    continue;
                stopwords_.insert(line);
            }
            ifs.close();
            assert(stopwords_.size());
            return true;
        }

        bool isDigital_(uint16_t uchar){
            uint16_t zero = '0';
            uint16_t nine = '9';
            if(zero <= uchar && uchar <= nine)
                return true;
            return false;
        }

        bool isAlpha_(uint16_t uchar){
            uint16_t a = 'a';
            uint16_t z = 'z';
            uint16_t A = 'A';
            uint16_t Z = 'Z';
            if((a <= uchar && uchar <= z) || (A <= uchar && uchar <= Z))
                return true;
            return false;
        }
        
        bool isAlphaNumberic_(const std::string& term){
            uint16_t uchar;
            std::string nstr = boost::replace_all_copy(term, ".", "");
            std::string::size_type i;
            for(i = 0; i < nstr.size(); ++i){
                if(isDigital_(nstr[i]) || isAlpha_(nstr[i]))
                    continue;
                return false;
            }
            return true;
        }

        // Check term is need to be cleaned or not, rules:
        //  1)digit, "123"
        //  2)alphanumeric, "cz1234"
        //  3)alphabet, "cwhk"
        //  4)single word, "吃"
        //  5)digit with dot, "123.45"
        //  6)term is too long , specific length = 10 * 3 (10 word)
        bool isNeedClean_(const std::string& term){
            if(term.empty())
                return true;
            // length > 30
            if(term.length() < 4 || term.length() > 31)
                return true;
            // is digital or numberic or digit with dot 
            if(isAlphaNumberic_(term))
                return true;

            return false;
        }

        void termsClean_()
        {
            std::cout << "[Info] Original Vocabulary size : " << vocab_.size() << std::endl;
            VocabType::iterator it;
            for(it = vocab_.begin(); it != vocab_.end();){
                if(stopwords_.end() != stopwords_.find(it->first) || isNeedClean_(it->first))
                    it = vocab_.erase(it);
                else
                    ++it;
            }
            std::cout << "[Info] After cleaning Vocabulary size : " << vocab_.size() << std::endl;
        }
    
    public:
        VocabModel(const std::string& dir, const std::string& sampleDir)
          :dictDir_(dir),sampleDir_(sampleDir),posDocCnt_(0),negDocCnt_(0)
        {
            if(!Zeus::nlp::SegmentWrapper::get()->loadDictFiles(dictDir_)){
                std::cout << "Resources load failed, resources path : " << dictDir_ << std::endl;
                return;
            }

            if(!loadStopWords_((dictDir_+"/stop_words.utf8"))){
                std::cout << "Stop words load failed!\n";
                return;
            }
        }

        ~VocabModel()
        {
        }

        // dedup: if it's true means you want to remove the same words in line.
        void segment(const std::string& line, std::vector<std::string>& token, bool dedup=true){
            token.clear();
            std::string nstr = normalize_(line);
            Zeus::nlp::SegmentWrapper::get()->segment(nstr, token);
            if(dedup){
                // Remove duplication
                std::set<std::string> set_(token.begin(), token.end());
                token.clear();
                std::copy(set_.begin(), set_.end(), std::back_inserter(token));
            }
        }
        // To extend token terms based on bigram
        // before extension: t1,t2,t3 
        // after extension: t1,t2,t3,t1_t2,t2_t3
        void bigramModel(std::vector<std::string>& token){
            std::vector<std::string> tmp(token);
            token.clear();
            std::size_t i, j, size = tmp.size();
            std::string bigram;
            for(i = 0; i < size; ++i){
                // clean
                if(stopwords_.end() != stopwords_.find(tmp[i]) || isNeedClean_(tmp[i]))
                    continue;
                token.push_back(tmp[i]);
                j = i + 1;
                if(j < size){
                    bigram = tmp[i] + tmp[j];
                    token.push_back(bigram);
                }
            }
#ifdef DEBUG
            std::cout << "[bigramModel] original size: " << size << ",bigram extension size: " << token.size() << std::endl;
#endif
        }
        // Get all words and it's information tf, df
        void getWordsInfo(const std::string& filename, VocabType& vocab,std::size_t& lineCount){
            lineCount = 0;
            if(filename.empty()){
                std::cout << "File " << filename << "is not exists!\n";
                return;
            }
            std::ifstream ifs(filename.c_str());
            if(!ifs.is_open()){
                std::cout << "Open " << filename << "failed!\n";
                return;
            }
            std::string line;
            std::vector<std::string> token;
            while(getline(ifs, line)){
                uint32_t pos = line.find('\t');
                if(pos == std::string::npos)
                    continue;
                std::string label = line.substr(0,pos);
                std::string content = line.substr(pos+1);
                segment(content, token);
                bigramModel(token);
                insertToVocab(token, vocab, label);
                lineCount += 1;
            }
            ifs.close();
        }
        
        // Insert word information into global vocabulary
        void insertToVocab(const std::vector<std::string>& vec, VocabType& vocab, const std::string& label){
            VocabType::iterator it;
            for(std::size_t i = 0; i < vec.size(); ++i){ 
                it = vocab.find(vec[i]);
                if(vocab.end() == it){
                    TermInfoType info = {0,0,1,0.0};
                    if(label == posLabel)
                        info.A += 1;
                    else if(label == negLabel)
                        info.B += 1;
                    vocab[vec[i]] = info;
                } else {
                    if(label == posLabel)
                        it->second.A += 1;
                    else if(label == negLabel)
                        it->second.B += 1;
                    it->second.tf += 1;
                }
            }
        }

        void genPosVocab(const std::string& filename){
            std::cout << "[Info] Start vocabulary generation from \"" << filename << "\" ...\n";
            getWordsInfo(filename, vocab_, posDocCnt_);
            std::cout << "[Info]  Vocabulary generation completed.\n";
        }
        
        void genNegVocab(const std::string& filename){
            std::cout << "[Info] Start vocabulary generation from \"" << filename << "\" ...\n";
            getWordsInfo(filename, vocab_, negDocCnt_);
            std::cout << "[Info] Vocabulary generation completed.\n";
        }
        
        void genVocab(){
            genPosVocab(sampleDir_+"/samples.pos");
            genNegVocab(sampleDir_+"/samples.neg");
            termsClean_();
            featureSelection();
            flush();
        }
       
        // Select TopK feaures
        void getFeatures(const std::string& sampleDir, std::vector<FeatureType>& featVec,const std::size_t TopK=1000){
            genPosVocab(sampleDir+"/samples.pos");
            genPosVocab(sampleDir+"/samples.neg");
            termsClean_();
            featureSelection();
            std::vector<FeatureType> vec(feature_.begin(), featureVec_.begin()+TopK);
            featVec.swap(vec);
        }

        // To caculate chi-square test value and select features
        // TODO: optimization
        void featureSelection(){
            VocabType::iterator it;
            featureVec_.resize(vocab_.size());
            std::size_t i=0;
            for(it = vocab_.begin(); it != vocab_.end(); ++it,++i){
                std::size_t C, D;
                C = (posDocCnt_) - it->second.A;
                D = (negDocCnt_) - it->second.B;
                float r = Zeus::FeatureSelect::CHiSquare<std::size_t, float>(it->second.A, it->second.B, C, D);
                float w = Zeus::FeatureSelect::TFIDF<std::size_t, float>(it->second.tf, (posDocCnt_ + negDocCnt_));
                r = (float)r * w;
                featureVec_[i] = std::make_pair(it->first, w);
                it->second.score = w;
                termScore_[it->first] = r;
            }
             
            // Sort
            std::sort(featureVec_.begin(), featureVec_.end(), Zeus::SORT<std::string, float>::sortDescendBySecond);
        }

        // TODO: serialization
        void flush(){
            std::ofstream ofs((sampleDir_+"/features").c_str());
            float w;
            boost::unordered_map<std::string, float>::iterator it;
            for(uint32_t i = 0; i < featureVec_.size(); ++i){
                it = termScore_.find(featureVec_[i].first);
                if(it == termScore_.end())
                    w = 0.0;
                else 
                    w = it->second;
                ofs << featureVec_[i].first << "," << featureVec_[i].second << "," << w << std::endl;
            }
            ofs.clear();
            ofs.close();
        }       
        
        // Clear all occupy memory
        void clear(){
            std::vector<FeatureType> tmp;
            featureVec_.swap(tmp);
            termScore_.clear();
            vocab_.clear();
        }
};


} // namespace nlp
} // namespace Zeus


#endif // genFeature.h
