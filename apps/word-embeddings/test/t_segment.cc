/*************************************************************************
 @ File Name: t_segment.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Tue 14 Jun 2016 06:04:07 PM CST
 ************************************************************************/
#include "nlp/segment/segmentWrapper.h"

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cout <<"Usage:\n\t exec-file infile outfile\n";
        return -1;
    }
    std::string infile = argv[1];
    std::string outfile = argv[2];
    std::string dict("../../../resource/prod");
    boost::shared_ptr<SegmentWrapper> segWrapper;
    segWrapper.reset(new SegmentWrapper(dict));
    std::ifstream ifs(infile.c_str());
    if(!ifs.is_open()){
        std::cout << "Open filename failed!\n";
        return -1;
    }
    std::ofstream ofs(outfile.c_str());
    if(!ofs.is_open()){
        std::cout << "Open output file failed!\n";
        return -1;
    }

    std::string line;
    std::vector<std::string> tokens;
    std::size_t cnt=0;
    while(getline(ifs, line)){
        if(line.empty())
            continue;
        segWrapper->segment(line, tokens, false);
        for(uint32_t i = 0; i < tokens.size(); ++i){
            if(segWrapper->isNeedClean(tokens[i]))
                continue;
            cnt++;
            ofs << tokens[i] << " ";
        }
    }
    ifs.close();
    ofs.close();
    std::cout << "Corpus process completed lexicon size: " << cnt << std::endl;

    return 0;
}

