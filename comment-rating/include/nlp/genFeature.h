/*************************************************************************
 @ File Name: genFeature.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 15 Apr 2016 10:49:33 AM CST
 ************************************************************************/
#ifndef ZEUS_NLP_GENFEATURE_H
#define ZEUS_NLP_GENFEATURE_H

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

// TF, DF combine type
typedef std::pair<uint32_t, uint32_t> TF_DFType;

// Vocabulary type
// Structure: term, tf, df
typedef boost::unordered_map<std::string, TF_DFType> VocabType;

// Feature type
// Structure: term, score/weight
typedef std::vector<Zeus::SORT<std::string, float>::VectorPair > FeatureType;

enum{negitive=0,positive};


class GenFeature
{
    private:

        std::string dictDir_;
        std::string sampleDir_;
        
        // total document number
        uint32_t posDocCnt_;
        uint32_t negDocCnt_;

        // Store all the term wiht tf and df
        VocabType posVocab_;
        VocabType negVocab_;
        VocabType medVocab_;
        
        // term-score dictionary
        boost::unordered_map<std::string, float> posTermScore_;
        boost::unordered_map<std::string, float> negTermScore_;

        // Stop words
        boost::unordered_set<std::string> stopwords_;

        // Selected Feature
        FeatureType posFeatureVec_; // positive sample features
        FeatureType negFeatureVec_; // negative sample features

        // Normalization, to lower case, remove white space at head and tail 
        std::string normalize_(const std::string& line){
            std::string nline(line);

            boost::algorithm::to_lower(nline);
            boost::algorithm::trim(nline);

            return nline;
        }
        
        // dedup: if it's true means you want to remove the same words in line.
        void segment_(const std::string& line, std::vector<std::string>& token, bool dedup=true){
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

        // Check term is need to be cleaned or not
        // clean rules: > digit, "123"
        //              > alphanumeric, "cz1234"
        //              > alphabet, "cwhk"
        //              > single word, "吃"
        //              > digit with dot, "123.45"
        //              > term is too long , specific length = 10 * 3 (10 word)
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
    
    public:
        GenFeature(const std::string& dir, const std::string& sampleDir)
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

        ~GenFeature()
        {
        }

        // Get all words and it's information tf, df
        void getWordsInfo(const std::string& filename, VocabType& vocab,uint32_t& lineCount){
            vocab.clear();
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
                std::string content = line.substr(pos+1);
                segment_(content, token);
                insertToVocab(token, vocab);
                lineCount += 1;
            }
            ifs.close();
        }
        
        // Insert word into vocabulary
        void insertToVocab(const std::vector<std::string>& vec, VocabType& vocab){
            VocabType::iterator it;
            for(uint32_t i = 0; i < vec.size(); ++i){
                
                it = vocab.find(vec[i]);
                if(vocab.end() == it){
                    vocab[vec[i]] = std::make_pair(1,1);
                } else {
                    it->second.first += 1; // tf increase
                    it->second.second += 1; // df increase
                }
            }
        }

        void genPosVocab(const std::string& filename){
            std::cout << "[Info] Start positive vocabulary generation...\n";
            getWordsInfo(filename, posVocab_, posDocCnt_);
            std::cout << "\tOriginal Positive Vocabulary size : " << posVocab_.size() << std::endl;
            // clear stop words
            VocabType::iterator it;
            for(it = posVocab_.begin(); it != posVocab_.end();){
                if(stopwords_.end() != stopwords_.find(it->first) || isNeedClean_(it->first))
                    it = posVocab_.erase(it);
                else
                    ++it;
            }
            std::cout << "\tAfter removing stop words Positive Vocabulary size : " << posVocab_.size() 
                      << "\tTotal documnet number: " << posDocCnt_ <<std::endl;
            std::cout << "[Info] Positive vocabulary generation completed!\n";
        }

        void genNegVocab(const std::string& filename){
            std::cout << "[Info] Start negative vocabulary generation...\n";
            getWordsInfo(filename, negVocab_, negDocCnt_);
            std::cout << "\tOriginal Positive Vocabulary size : " << negVocab_.size() << std::endl;
            // clear stop words
            VocabType::iterator it;
            for(it = negVocab_.begin(); it != negVocab_.end();){
                if(stopwords_.end() != stopwords_.find(it->first) || isNeedClean_(it->first))
                    it = negVocab_.erase(it);
                else
                    ++it;
            }
            std::cout << "\tAfter removing stop words Positive Vocabulary size : " << negVocab_.size() 
                        << "\tTotal document number: " << negDocCnt_ << std::endl;
            std::cout << "[Info] Negative vocabulary generation completed!\n";
        }
        
        void genVocab(){
            genPosVocab(sampleDir_+"/samples.pos");
            genNegVocab(sampleDir_+"/samples.neg");
            featureSelection();
            flush();
        }
        
        // To caculate chi-square test value and select features
        // TODO: optimization
        void featureSelection(){
            VocabType::iterator it,fit;
            posFeatureVec_.resize(posVocab_.size());
            negFeatureVec_.resize(negVocab_.size());
            boost::unordered_map<std::string, uint32_t> dfInPos;

            // Negative
            uint32_t i;
            for(i=0,it=negVocab_.begin(); it != negVocab_.end(); ++it, ++i){
                uint32_t A, B, C, D;
                // Assign values for A,B,C,D
                A = it->second.second;
                fit = posVocab_.find(it->first);
                if(fit != posVocab_.end()){
                    B = fit->second.second;
                    dfInPos[it->first] = A;
                } else {
                    B = 0;
                }
                C = log(negDocCnt_+1) - A;
                D = log(posDocCnt_+1) - B;
                
                // CHi-Square
                float r = Zeus::FeatureSelect::CHiSquare<uint32_t, float>(A, B, C, D);
                negFeatureVec_[i] = std::make_pair(it->first, r);
                // TF-IDF
                float w = Zeus::FeatureSelect::TFIDF<uint32_t, float>(it->second.first, negDocCnt_);
                negTermScore_[it->first] = w;
            }
            // Positive
            boost::unordered_map<std::string, uint32_t>::iterator pit;
            for(i=0,it=posVocab_.begin(); it != posVocab_.end(); ++it, ++i){
                uint32_t A, B, C, D;
                // Assign values for A, B, C, D
                A = it->second.second;
                pit = dfInPos.find(it->first);
                if(pit != dfInPos.end())
                    B = pit->second;
                else
                    B = 0;
                C = log(posDocCnt_+1) - A;
                D = log(negDocCnt_+1) - B;
                
                // CHi-Square
                float r = Zeus::FeatureSelect::CHiSquare<uint32_t, float>(A, B, C, D);
                posFeatureVec_[i] = std::make_pair(it->first, r);
                // TF-IDF
                float w = Zeus::FeatureSelect::TFIDF<uint32_t, float>(it->second.first, posDocCnt_);
                posTermScore_[it->first] = w;
            }

            // Sort
            std::sort(negFeatureVec_.begin(), negFeatureVec_.end(), Zeus::SORT<std::string, float>::sortDescendBySecond);
            std::sort(posFeatureVec_.begin(), posFeatureVec_.end(), Zeus::SORT<std::string, float>::sortDescendBySecond);
        }

        void genFeatures(){
            VocabType::iterator it;
            posFeatureVec_.resize(posVocab_.size());
            negFeatureVec_.resize(negVocab_.size());
            uint32_t i;
            for(i=0,it = posVocab_.begin(); it != posVocab_.end(); ++it,++i){
                float w = Zeus::FeatureSelect::TFIDF<uint32_t, float>(it->second.first, /*it->second.second,*/ posDocCnt_ /*, posVocab_.size()*/);
                posFeatureVec_[i] = std::make_pair(it->first, w);
            }
            
            std::sort(posFeatureVec_.begin(), posFeatureVec_.end(), Zeus::SORT<std::string, float>::sortDescendBySecond);

            for(i=0,it = negVocab_.begin(); it != posVocab_.end(); ++it,++i){
                float w = Zeus::FeatureSelect::TFIDF<uint32_t, float>(it->second.first, /*it->second.second,*/ negDocCnt_ /*, posVocab_.size()*/);
                negFeatureVec_[i] = std::make_pair(it->first, w);
            }
            std::sort(negFeatureVec_.begin(), negFeatureVec_.end(), Zeus::SORT<std::string, float>::sortDescendBySecond);
        }

        // TODO: serialization
        void flush(){
            std::ofstream ofs((sampleDir_+"/feature.pos").c_str());
            float w;
            boost::unordered_map<std::string, float>::iterator it;
            for(uint32_t i = 0; i < posFeatureVec_.size(); ++i){
                it = posTermScore_.find(posFeatureVec_[i].first);
                if(it == posTermScore_.end())
                    w = 0.0;
                else 
                    w = it->second;
                ofs << posFeatureVec_[i].first << "," << posFeatureVec_[i].second << "," << w << std::endl;
            }
            ofs.clear();
            ofs.close();

            ofs.open((sampleDir_+"/feature.neg").c_str());
            for(uint32_t i = 0; i < negFeatureVec_.size(); ++i){
                it = negTermScore_.find(negFeatureVec_[i].first);
                if(it == negTermScore_.end())
                    w = 0.0;
                else
                    w = it->second;
                ofs << negFeatureVec_[i].first << "," << negFeatureVec_[i].second << "," << w << std::endl;
            }
            ofs.clear();
            ofs.close();
        }

        
};


} // namespace nlp
} // namespace Zeus


#endif // genFeature.h
