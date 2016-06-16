/*************************************************************************
 @ File Name: user_profile.hpp
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 16 May 2016 09:57:00 AM CST
 ************************************************************************/
#ifndef _USER_PROFILE_HPP
#define _USER_PROFILE_HPP

#include <iostream>
#include <boost/unordered_map.hpp>
#include "segmentWrapper.h"

// User Information type
struct UserInfoType
{
    boost::unordered_map<uint32_t, float> cate_map;
    std::vector<std::string> titles;
    std::vector<std::pair<std::string, float> > features;
    uint32_t total_cid;
};


class ProfileCreate
{
    private:
        boost::unordered_map<std::string, UserInfoType> userInfo_;
        std::string resDir_;
        SegmentWrapper* segWrapper_;

    public:
        ProfileCreate(const std::string& resDir)
        :resDir_(resDir), segWrapper_(NULL){
            segWrapper_ = new SegmentWrapper(resDir_+"/dict");
            // load resources
            loadProfile();
        }
        
        /*
        ProfileCreate(const std::string& resDir
                    ,const std::vector<int>& itemIDs
                    ,const std::vector<std::string>& titles)
        :resDir_(resDir) ,segWrapper_(NULL){
            segWrapper_ = new SegmentWrapper(resDir_+"/dict");
            // load resources
            loadProfile();

            // Pre-Segmentation
            if(itemIDs.empty() || titles.empty())
                return;
            if(itemIDs.size() != titles.size())
                return;
            segment(titles, itemIDs);
        }*/

        ~ProfileCreate(){
            delete segWrapper_;
            segWrapper_ = NULL;
        }
       
        // Segmentation and store itemID , tokens
       /* void segment(const std::vector<std::string>& titles, const std::vector<int>& itemIDs){
            for(uint32_t i = 0; i < titles.size(); ++i){
                segWrapper_->segment(titles[i], itemIDs[i]);
            }
        }*/
        
        // Interface:
        // String segmentation
        void segment(const std::string& title, std::vector<std::string>& token){
            token.clear();
            token = segWrapper_->segment(title);
        }

        // Interface:
        // to update new item
       // void itemUpdates(const std::string& title, int itemIDs){
       //     if(title.empty())
       //         return;
       //     segWrapper_->segment(title, itemIDs);
       // }
        // Interface:
        // Compute the similarity of item-item_list
       /* void item2ItemsSim(int itemID, const std::vector<int>& itemIDs, std::vector<std::string>& rankList){
            rankList.clear();
            if(itemIDs.empty())
                return;
            std::vector<std::pair<int, double> > itemIdScore;
            itemIdScore.resize(itemIDs.size());
            for(uint32_t i = 0; i < itemIDs.size(); ++i){
                double s = computeContentSim(itemID, itemIDs[i]); 
                itemIdScore[i] = std::make_pair(itemIDs[i], s);
            }
            //sort
            std::sort(itemIdScore.begin(), itemIdScore.end(), SORT<int, double>::sortDescendBySecond);
            rankList.resize(itemIdScore.size());
            std::stringstream ss;
            for(uint32_t i = 0; i < itemIdScore.size(); ++i){
                ss << itemIdScore[i].first << ":" << itemIdScore[i].second;
                std::string res(ss.str());
                rankList[i] = res;
                ss.str("");
            }
        }*/
        // Interface : 
        // Recommend items based on user-profile
        void recommend(const std::string& userID
                    ,const std::vector<int>& itemIds
                    ,const std::vector<std::vector<std::string> >& tokenList
                    ,std::vector<std::string>& rankList
                    ,const int Topk=50){
            rankList.clear();
            boost::unordered_map<std::string, UserInfoType>::iterator uit;
            uit = userInfo_.find(userID);
            if(uit == userInfo_.end())
                return;
            std::vector<std::pair<std::string, float> > features;
            std::vector<std::pair<int, double> >result;
            features = uit->second.features;
            double sim=0.0;
            //std::vector<int> itemIds = segWrapper_->getItemIDs();
            result.resize(itemIds.size());
            for(uint32_t i = 0; i < itemIds.size(); ++i){
                similarityCalculate(tokenList[i], features, sim);
                result[i] = std::make_pair(itemIds[i], sim);
            }
            // sort
            std::sort(result.begin(), result.end(), SORT<int, double>::sortDescendBySecond);
            uint32_t topk;
            if(Topk > result.size())
                topk = result.size();
            else
                topk = Topk;
            std::stringstream ss;
            for(uint32_t i = 0; i < topk; ++i){
                {
                    ss << result[i].first << ":" << result[i].second;
                    std::string res(ss.str());
                    ss.str("");
                    rankList.push_back(res);
                }
            }
        }

        // Interface:
        // Compute the similarity when given two item id
        double computeContentSim(const std::vector<std::string>& token1
                                ,const std::vector<std::string>& token2){
            return segWrapper_->computeContentSim(token1, token2);
        }

        // Creat profile based on user order info
        // Calculate the proportion of every cid in cate_map
        // Extract feature terms 
        void profileCreate(uint32_t Topk=50){
            boost::unordered_map<std::string, UserInfoType>::iterator uit;
            for(uit = userInfo_.begin(); uit != userInfo_.end(); ++uit){
                // calcualte cid and it's weight
                UserInfoType &utmp = uit->second;
                boost::unordered_map<uint32_t, float>::iterator cit;
                float sum = 0.1;
                for(cit = utmp.cate_map.begin(); cit != utmp.cate_map.end(); ++cit){
                    sum += cit->second;
                }
                for(cit = utmp.cate_map.begin(); cit != utmp.cate_map.end(); ++cit){
                    cit->second = (float)cit->second / sum;
                }
                // extract features and it's weight
                boost::unordered_map<std::string, float> featTmp;
                boost::unordered_map<std::string, float>::iterator tit;
                float tokenSum = 0.1;
                for(uint32_t i = 0; i < utmp.titles.size(); ++i){
                    std::vector<std::string> token;
                    segWrapper_->subSegment(utmp.titles[i], token, false);
                    segWrapper_->bigramModel(token);
                    for(uint32_t j = 0; j < token.size(); ++j){
                        tit = featTmp.find(token[j]);
                        if(tit != featTmp.end())
                            tit->second += 1;
                        else
                            featTmp[token[j]] = 1;
                    }
                }
                
                // total term number
                for(tit = featTmp.begin(); tit != featTmp.end(); ++tit)
                    tokenSum += tit->second;

                // calculate term weight and sort
                std::vector<std::pair<std::string, float> > tokenWeight;
                tokenWeight.resize(featTmp.size());
                uint32_t k;
                for(tit = featTmp.begin(), k=0; tit != featTmp.end() && k < tokenWeight.size(); ++tit,++k){
                    tit->second = (float)tit->second / tokenSum;
                    tokenWeight[k] = std::make_pair(tit->first, tit->second);
                }
                std::vector<std::string> ().swap(utmp.titles);
                // sort
                std::sort(tokenWeight.begin(), tokenWeight.end(),SORT<std::string, float>::sortDescendBySecond);
                // Select topk
                uint32_t topk;
                if(Topk >= tokenWeight.size())
                    topk = tokenWeight.size();
                else
                    topk = Topk;
                
                std::vector<std::pair<std::string, float> > result(tokenWeight.begin(), tokenWeight.begin()+topk);
                utmp.features.swap(result);
            }
        }

        void store(){
            boost::unordered_map<std::string, UserInfoType>::iterator uit;
            boost::unordered_map<uint32_t, float>::iterator cit;
            std::ofstream ofs("user.profile");
            uit = userInfo_.begin();
            for(;uit != userInfo_.end(); ++uit){
                ofs << uit->first << "\t";
                cit = uit->second.cate_map.begin();
                for(;cit != uit->second.cate_map.end(); ++cit){
                    ofs << cit->first << ":" << cit->second << ",";
                }
                ofs << "\t";
                for(uint32_t i = 0; i < uit->second.features.size(); ++i)
                    ofs << uit->second.features[i].first << ":" << uit->second.features[i].second << ",";
                ofs << std::endl;
            }
            ofs.close();
        }   

        // Parse key:value 
        bool keyValueParse(const std::string key_value, std::string& key, std::string& value){
            std::string::size_type pos = key_value.find(':');
            if(pos == std::string::npos)
                return false;
            key = key_value.substr(0,pos);
            value = key_value.substr(pos+1);
            return true;
        }
        // Load the created profile
        void loadProfile(){
            
            std::ifstream ifs((resDir_+"/dict/user.profile").c_str());
            if(!ifs.is_open()){
                std::cerr << "Open user profile failed!\n";
                return;
            }
            std::string line;
            std::vector<std::string> contentVec;
            while(getline(ifs, line)){
                if(line.empty())
                    continue;
                boost::split(contentVec, line, boost::is_any_of("\t"));
                if(contentVec.size() != 3)
                    continue;
                std::string userID = contentVec[0];
                std::string category = contentVec[1];
                std::string features = contentVec[2];
                UserInfoType utmp;
                // parse category and it's weight
                std::vector<std::string> key_value;
                boost::split(key_value, contentVec[1], boost::is_any_of(","));
                if(key_value.empty())
                    continue;
                for(uint32_t i = 0; i < key_value.size(); ++i){
                    std::string key, value;
                    uint32_t cid;
                    float weight;
                    if(!keyValueParse(key_value[i], key, value))
                        continue;
                    try{
                        cid = boost::lexical_cast<uint32_t>(key);
                        weight = boost::lexical_cast<float>(value);
                    }catch(...){
                        continue;
                    }
                    utmp.cate_map[cid] = weight;
                }
                // Parse feature and it's weight
                boost::split(key_value, contentVec[2], boost::is_any_of(","));
                if(key_value.empty())
                    continue;
                for(uint32_t i = 0; i < key_value.size(); ++i){
                    std::string key, value;
                    float weight;
                    if(!keyValueParse(key_value[i], key, value))
                        continue;
                    try{
                        weight = boost::lexical_cast<float>(value);
                    }catch(...){
                        continue;
                    }
                    utmp.features.push_back(std::make_pair(key, weight));
                }
                userInfo_[userID] = utmp;
            }
            ifs.close();
            std::cout << "User profile loaded size: " << userInfo_.size() << std::endl;
        }
        // Calcuate similarity for one title and one user's features
        void similarityCalculate(const std::vector<std::string>& token
                    ,const std::vector<std::pair<std::string, float> >& features
                    ,double& sim){
            if(token.empty() || features.empty()){
                sim = 0.0;
                return;
            }
            //std::vector<std::string> token;
            //segWrapper_->getTokens(itemID, token);

            std::vector<std::string> feat, result;
            feat.resize(features.size());
            for(uint32_t i = 0; i < features.size(); ++i)
                feat[i] = features[i].first;
            segWrapper_->intersect(token, feat, result);
            sim = ((double)result.size()/token.size()) * ((double)result.size()/feat.size());
        }

        /*
        void getItemIDs(std::vector<int>& itemIds){
            itemIds = segWrapper_->getItemIDs();
        }*/
};


#endif // user_profile.hpp
