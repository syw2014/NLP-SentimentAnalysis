/*************************************************************************
 @ File Name: t_user_profile.cc
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Mon 16 May 2016 08:09:11 PM CST
 ************************************************************************/
#include <iostream>
#include "profiling_content_sim.hpp"

int main(int argc, char* argv[])
{
    std::string resDir = "../resource";
    ProfileCreate *create_ = NULL;
    //std::cout << "Start to load resources...\n";
    //create_ =  new ProfileCreate(resDir);
    //std::cout << "Resources loaded!\n";

    std::vector<std::string>  titles, rankList, token;
    std::vector<std::vector<std::string> > tokenList;
    std::vector<int> itemID;
    itemID.push_back(2);
    itemID.push_back(1);
    itemID.push_back(23);
    itemID.push_back(56);
    itemID.push_back(77);
    itemID.push_back(45);
    itemID.push_back(38);

    titles.push_back("ROBERTSON'S 罗拔臣 英国杏桃果酱 3周岁以上食用 340克/罐 2罐装");
    titles.push_back("Mederma  淡疤淡化妊娠纹疤痕修复凝胶50克 美国直邮");
    titles.push_back("AHC 眼霜第四代30ml 保湿紧致去眼袋细纹黑眼圈 韩国直邮");
    titles.push_back("清风3层10包原木纯品纸手帕");
    titles.push_back("中兴WP826A电信CDMA天翼4G家用办公无线座机固话插卡电话老人机");
    titles.push_back("Philips飞利浦 电动剃须刀 AT891A");
    titles.push_back("飞利浦/Philips 电动剃须刀 PT870带弹出式修剪器浮动刀头全身水洗刮胡刀");
    std::string userID("bdda181458e94890ba8215095ee4211f");
    
    create_ =  new ProfileCreate(resDir);
    
    std::cout << "===========TEST1 TOKENIZE===========\n";
    tokenList.resize(itemID.size());
    for(uint32_t i = 0; i < itemID.size(); ++i){
        create_->segment(titles[i], token);
        tokenList[i] = token;
        std::cout << itemID[i] << ", size: " << token.size()<< std::endl;
    }
    std::cout << std::endl;
    

    std::cout << "===========TEST2 CONTENT SIM===========\n";
    // test compute content similarity
    //std::string t1 = "Philips飞利浦 电动剃须刀 AT891A";
    //std::string t2 = "飞利浦/Philips 电动剃须刀 PT870带弹出式修剪器浮动刀头全身水洗刮胡刀";
    
    int t1 = 56;
    int t2 = 77;
    double s = create_->computeContentSim(tokenList[3], tokenList[4]);
    std::cout << "Content similarity: " << s << std::endl;

    std::cout << "===========TEST3 USER RECOMMMENDATION===========\n";
    std::string tt = "中兴WP826A电信CDMA天翼4G家用办公无线座机固话插卡电话老人机";
    create_->recommend(userID, itemID, tokenList, rankList, 3);
    
    for(uint32_t i = 0; i < rankList.size(); ++i)
        cout << rankList[i] << ",";
    std::cout << std::endl;
    
    //std::cout << "===========TEST4 CONTENT SIM===========\n";
    // test compute content similarity
    //std::string t1 = "Philips飞利浦 电动剃须刀 AT891A";
    //std::string t2 = "飞利浦/Philips 电动剃须刀 PT870带弹出式修剪器浮动刀头全身水洗刮胡刀";
   /* 
    t1 = 56;
    t2 = 100;
    s = create_->computeContentSim(t1, t2);
    std::cout << "Content similarity: " << s << std::endl;

    std::cout << "===========TEST5 ITEM UPDATES===========\n";
    create_->item2ItemsSim(100, itemID, rankList);
    for(uint32_t i = 0; i < rankList.size(); ++i)
        cout << rankList[i] << ",";
    std::cout << std::endl;*/

    delete create_;

    return 0;
}

