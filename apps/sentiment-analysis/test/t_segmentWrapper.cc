/*************************************************************************
 @ File Name: t_segmentWapper.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 15 Apr 2016 06:01:19 PM CST
 ************************************************************************/
#include "nlp/segmentWrapper.h"
#include <ctime>

int main(int argc, char* argv[])
{
    clock_t start, end;
    std::string dictDir = "../../dict";
    start = clock();
    if(!Zeus::nlp::SegmentWrapper::get()->loadDictFiles(dictDir)){
        std::cout << "Resources loaded failed!\n";
    }
    end = clock();
    std::cout << "Loaded resources time: " << (double)(end - start)/CLOCKS_PER_SEC << "s.\n";
    
    std::string str = "【全球购】德国原装爱他美奶粉白金版奶粉800gpre段1段一段2段(6个月以上)";
    std::vector<std::string> vec;
    
    start = clock();
    uint32_t cnt = 10000;
    while(cnt--){
    Zeus::nlp::SegmentWrapper::get()->segment(str, vec);
    }
    end = clock();
    std::cout << "Segmentation time: " << (double)(end - start)/CLOCKS_PER_SEC << "s.\n";
    
    for(uint32_t i = 0; i < vec.size() ; ++i)
        std::cout << vec[i] << "//";
    std::cout << '\n';
    return 0;
}

