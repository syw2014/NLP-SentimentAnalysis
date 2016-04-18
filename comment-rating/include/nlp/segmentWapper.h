/*************************************************************************
 @ File Name: segmentWapper.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 15 Apr 2016 04:36:36 PM CST
 ************************************************************************/
#ifndef ZEUS_NLP_SEGMENT_WAPPER_H
#define ZEUS_NLP_SEGMENT_WAPPER_H

#include <vector>
#include <map>
#include <string>
#include <assert.h>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>

#include "pattern/singleton.hpp"
#include "cppjieba/Jieba.hpp"
#include "cppjieba/KeywordExtractor.hpp"

namespace Zeus
{
namespace nlp
{
    // Tokenize Class
    class cppjieba::Jieba;

    class SegmentWrapper
    {
        private:
            bool isDictLoaded_;
            boost::scoped_ptr<cppjieba::Jieba> segment_;

        public:

            static SegmentWrapper* get(){
                return DesignPattern::Singleton<SegmentWrapper>::get();
            }

            SegmentWrapper()
              :isDictLoaded_(false)
            {
            }

            ~SegmentWrapper()
            {
            }
            
            bool loadDictFiles(const std::string dictDir){
                if(isDictLoaded_)
                    return true;

                boost::filesystem::path dirPath(dictDir);
                assert(boost::filesystem::exists(dirPath));
                std::string dir(dictDir);
                try{
                    std::string usr_dict = dir + "/user.dict.utf8";
                    std::string hmm_model = dir + "/hmm_model.utf8";
                    std::string jieba_dict = dir + "/jieba.dict.utf8";
                    segment_.reset(new cppjieba::Jieba(jieba_dict.c_str(), hmm_model.c_str(), usr_dict.c_str()));
                }catch(const std::exception& ex){
                    std::cout << "Exception: " << ex.what() << ", dictDir: " << dictDir << std::endl;
                    return false;
                }

                isDictLoaded_ = true;
                std::cout << "Finished loading dictionary resources.\n";
                return true;
            }
            // segment with HMM
            void segment(const std::string& line, std::vector<std::string>& terms, bool hmm = true){
                segment_->Cut(line, terms, hmm);
            }
    };
    
} // namespace nlp
} // namespace Zeus

#endif // nlp/segmentWarpper.h
