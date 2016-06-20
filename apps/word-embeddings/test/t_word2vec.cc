/*************************************************************************
 @ File Name: t_word2vec.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 20 Jun 2016 10:55:23 AM CST
 ************************************************************************/
#include <iostream>
#include "nlp/w2v/distance.hpp"

int main(int argc, char* argv[])
{
    boost::shared_ptr<WordVectorPredict> pWordVec;
    std::string model = "../model/vectors.bin";
    pWordVec.reset(new WordVectorPredict(model));
    std::string word;
    std::vector<float> bestd;
    std::vector<std::string> bestw;
    uint32_t topk = 40;
    std::cout << "Input words:\n";
    while(cin >> word){
        pWordVec->GetNearestTerms(word, bestd, bestw, topk);
        for(uint32_t i = 0; i < topk;++i){
            std::cout << bestw[i] << "\t\t" << bestd[i] << std::endl;
        }
        std::cout << "Input words:\n";
    }
    
    return 0;
}

