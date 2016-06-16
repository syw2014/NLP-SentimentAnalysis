/*************************************************************************
 @ File Name: t_vocabModel.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 18 Apr 2016 05:43:07 PM CST
 ************************************************************************/
#include <iostream>
#include "nlp/vocabModel.h"

int main(int argc, char* argv[])
{
    std::string sampleDir = "../../samples";
    std::string dictDir = "../../../../resource/jieba";
    Zeus::nlp::VocabModel* gen = NULL;

    gen = new Zeus::nlp::VocabModel(dictDir, sampleDir);
    gen->genVocab();

    delete gen;
    return 0;
}

