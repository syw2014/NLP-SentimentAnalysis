/*************************************************************************
 @ File Name: t_rating.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Tue 31 May 2016 05:35:46 PM CST
 ************************************************************************/
#include "ml/rating.hpp"

int main(int argc, char* argv[])
{
    std::string featureDir = "../../samples";
    std::string dictDir = "../../../dict";
    std::string modelDir = "/work/SF1R_Proj/hobby/ml/liblinear/comment_scale.model";
    std::string line;

    ml::Rating* pRating = NULL;
    pRating = new ml::Rating(featureDir, dictDir, modelDir);
    std::vector<float> elem;
    double score;
  //  ml::FeatureType* pSample = NULL;
    std::cout << "Input string: \n";
    while(std::cin >> line){
        std::cout << "str: " << line << std::endl;
        pRating->PredictClassLabel(line, score);
        std::cout << "TEST SCORE: " << score << std::endl;
        std::cout << "Input string: \n";
    }
    
    delete pRating;
//    free(pSample);
    return 0;
}

