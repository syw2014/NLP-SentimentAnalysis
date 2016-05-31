/*************************************************************************
 @ File Name: t_predict.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 27 May 2016 04:02:35 PM CST
 ************************************************************************/
#include <iostream>
#include "ml/predict.hpp"

int main(int argc, char* argv[])
{
    std::string model_ = "/work/SF1R_Proj/hobby/ml/liblinear/comment_scale.model";
    std::string predict_file = "/work/SF1R_Proj/hobby/zeus/comment-rating/samples/test_data/comment_scale.t";

    ml::Predict* pPre_ = NULL;

    std::cout << "[INFO] Start to load trained model...\n";
    pPre_ = new ml::Predict(model_);
    std::cout << "[INFO] Trained model loaded!\n";


    std::cout << "[INFO] Start to predict...\n";
    pPre_->BatchPredict(predict_file);
    std::cout << "[INFO] Finished sample label predict!\n";

    delete pPre_;
    return 0;
}

