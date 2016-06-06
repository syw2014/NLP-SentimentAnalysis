/*************************************************************************
 @ File Name: t_vectorize.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Thu 12 May 2016 05:48:04 PM CST
 ************************************************************************/
#include <iostream>
#include "nlp/vectorize.hpp"

int main(int argc, char* argv[])
{
    std::string sampleDir = "../../samples";
    std::string dictDir = "../../../dict";
    Zeus::nlp::Vectorize* vectorize = NULL;

    vectorize = new Zeus::nlp::Vectorize(sampleDir, dictDir);
    //vectorize->getfeatures();
    vectorize->loadFeature();
    vectorize->vectorize();
   // vectorize->store();

    delete vectorize;
    vectorize = NULL;

    return 0;
}



