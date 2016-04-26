/*************************************************************************
 @ File Name: t_genFeature.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 18 Apr 2016 05:43:07 PM CST
 ************************************************************************/
#include <iostream>
#include "nlp/genFeature.h"

int main(int argc, char* argv[])
{
    std::string sampleDir = "../../samples";
    std::string dictDir = "../../../dict";
    Zeus::nlp::GenFeature* gen = NULL;

    gen = new Zeus::nlp::GenFeature(dictDir, sampleDir);
    gen->genVocab();

    delete gen;
    return 0;
}

