/*************************************************************************
 @ File Name: genFeature.h
 @ Method: 
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 15 Apr 2016 10:49:33 AM CST
 ************************************************************************/
#ifndef ZEUS_NLP_GENFEATURE_H
#define ZEUS_NLP_GENFEATURE_H

namespace Zeus
{
namespace nlp
{

// TF, DF combine type
typedef std::pair<uint32_t, uint32_t> TF_DFType;

// Lexicon type
// Structure: term, tf, df
typedef boost::unordered_map<std::string, TF_DFType> LexiconType;

// Feature type
// Structure: term, score/weight
typedef std::vector<std::pair<std::string, float> > FeatureType;

enum{negitive=0,positive};


class GenFeature
{
    private:
        // Store all the term wiht tf and df
        LexiconType posLexicon_;
        LexiconType negLexicon_;
        LexiconType medLexicon_;

        // Selected Feature
        FeatureType posFeatureVec;
        FeatureType negFeatureVec;
        
        
};
} // namespace nlp
} // namespace Zeus
#endif // genFeature.h
