/*************************************************************************
 @ File Name: distance.hpp
 @ Method: Consine distance
 @ Author: Jerry Shi
 @ Mail: jerryshi0110@gmail.com
 @ Created Time: Fri 17 Jun 2016 09:37:29 AM CST
 ************************************************************************/
#ifndef NLP_W2V_DISTANCE_HPP_
#define NLP_W2V_DISTANCE_HPP_

// An encapsulated class to computate cosine similarity between two vectors,
// firstly to load the trained word2vec model,then segment text into tokens,
// find all the corresponding word vector for every token if exists, now you
// can compute the consine distance of two tokens based on their word vector.
// But there is no general strategy to represent sentence vector based on 
// word vector.
//
// TODO: Optimizate sentence vector representation


#include <iostream>
#include "nlp/segment/segmentWrapper.h"

// C headers
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

const long long max_size = 2000; // max length of strings
const long long N = 40;          // number of closest words
const long long max_w = 50;      // max length of vocabulary entries


// Word vector compuate, you can use this class to generate a words vector for
// a specific word, but the model is limited by the corpus size, if the training
// corpus is big enough you can find all the word vector you need.Based on the 
// word vector you can do semantic analysing. Usage:
// 
// WordVectorPredict wPre(model_path);
// std::string word;
// std::vector<float> bestd;
// std::vector<std::string> bestw;
// wPre.GetNearestTerms(word, bestd, bestw, 40);
class WordVectorPredict
{
    private:
        std::string model_;      // Trained model file name
        long long words_num_;    // Total words number in trained model, also is the size of vocabulary
        long long vector_size_;  // Word vector size, set when training model
       // long long word_index_;   // Store word index in word dictionary
        float *M;                // Memory for all  elemnets, it's size = vocabulary size * dimension * type size(float)              
        char *vocab;             // The memory for all terms, it's size = vocabulary size * term length * type size(char)
       
        boost::shared_ptr<SegmentWrapper> segWrapper_;
        std::string dict_;       // Tokenizer dictionary path
        
        // Load model from file, parse the parameters 
        bool LoadModel_(){
            if(model_.empty()){
                std::cout << "Model file is not exists!\n";
                return false;
            }
            FILE* ifs;
            ifs = fopen(model_.c_str(), "rb");
            if(ifs == NULL){
                std::cout << "Open model file error!\n";
                return false;
            }
            fscanf(ifs, "%lld", &words_num_);  // get the vocabulary size from model file
            fscanf(ifs, "%lld", &vector_size_);   // obtain the dimension of term from model file
            std::cout << "TEST words num: " << words_num_ << "\t vector size: " << vector_size_ << std::endl; 
            vocab = (char *)malloc((long long)words_num_ * max_w * sizeof(char)); // Alloc memory for all terms
            if(vocab == NULL){
                std::cout << "Alloc memory: "<< (long long)words_num_ * max_w * sizeof(char) / 1048576 
                            <<"MB for terms failed!\n";
                return false;
            }
            M = (float *)malloc((long long)words_num_ * (long long)vector_size_ * sizeof(float)); // Alloc memory for all elements
            if(M == NULL){
                std::cout << "Alloc memory: " << (long long)words_num_ * vector_size_ * sizeof(float) / 1048576
                            << "MB failed for all elements.\n";
                return false;
            }
            
            // Load all terms from model
            for(std::size_t b = 0; b < words_num_; ++b){
                std::size_t a = 0;
                while(1){
                    vocab[b * max_w + a] = fgetc(ifs); // readline 
                    if(feof(ifs) || (vocab[b * max_w + a] == ' '))
                        break;
                    if((a < max_w) && (vocab[b * max_w + a] != '\n'))
                        a++;
                }
                vocab[b * max_w + a] = 0;
                for(a = 0; a < vector_size_; ++a)
                    fread(&M[a + b * vector_size_], sizeof(float), 1, ifs);
                float len = 0;
                // Compute the square for every dimension and calcuate the sum for cosine compuataion
                // Trick,pre-process for cosine
                for(a = 0; a < vector_size_; ++a)
                    len += M[a + b * vector_size_] * M[a + b*vector_size_];
                len = sqrt(len);
                for(a = 0; a < vector_size_; ++a)
                    M[a + b*vector_size_] /= len;
            }
            fclose(ifs);
        }

    public:
        WordVectorPredict(const std::string& model, const std::string& dict)
            :model_(model), words_num_(0), vector_size_(0)
            ,M(NULL), vocab(NULL), dict_(dict){
                std::cout << "Start to load word2vec model...\n";
                LoadModel_();
                std::cout << "Word2vec model loaded!\n";

                std::cout << "Start to create segment wrapper...\n";
                if(!dict_.empty()) {
                    segWrapper_.reset(new SegmentWrapper(dict_));
                } else {
                    std::cout << "Create segment wraper failed!\n";
                    return;
                }
        }

        ~WordVectorPredict(){
            if (M != NULL) {
                delete M;
                M = NULL;
            }

            if (vocab != NULL) {
                delete vocab;
                vocab = NULL;
            }
        }
       
        // Given a word ,find it's word vector and index in the whole vocabulary
        // if exists, if not exists index will be -1, word vecor is empty.
        void GetWordVector(const std::string& word
                            ,long long index
                            ,std::vector<float>& vec){
            vec.clear();
            index = -1;
            if(word.empty())
                return;
            vec.resize(vector_size_); // resize vector to store the final vector
            char term[word.length()];
            long long a, b;
            strcpy(term, word.c_str());
            // search the term in vocabulary and record it's index if exists
            for(b = 0; b < words_num_; ++b)
                if(!strcmp(&vocab[b*max_w], term))
                    break;
            if(b == -1){
                std::cout << "Out of dictionary word!\n";
                return;
            }
           
           // Record word index in vocabulary
           index = b;

           // TODO: reduce reprocessing
           // initalize vector
           for(a = 0; a < vector_size_; ++a)
               vec[a] = 0;
           // consine pre-process
           for(a = 0; a < vector_size_; ++a){
               //std::cout << M[a+b*vector_size_] << std::endl;
               vec[a] += M[a + b*vector_size_];
           }
           float elem_square_sum = 0.0;
           for(a = 0; a < vector_size_; ++a){
               elem_square_sum += (float)vec[a] * vec[a];
           }
           elem_square_sum = (float)sqrt(elem_square_sum);
           // Trick to accelerate cosine computation
           // cosine = sum(xi*yi)/(|X||Y|)
           // cosine = sum (xi/|X|)*(yi/|Y|)
           for(a = 0; a < vector_size_; ++a){
               if (elem_square_sum == 0) {
                   vec[a] = 0;
                   continue;
               } else {
                    vec[a] /= (float)elem_square_sum;
               }
           }
        }
        
        // Computation the similarity of two strings
        float Similarity(const std::string& lstr, const std::string& rstr){
            std::vector<float> lvec(vector_size_, 0.0);
            std::vector<float> rvec(vector_size_, 0.0);
            SentenceVector(lstr, lvec);
            SentenceVector(rstr, rvec);
            double sim = 0.0;
            if(lvec.empty() || rvec.empty())
                return sim;
            sim = CosineDistance(lvec, rvec);
            return sim;
        }

        // To calcuate cosine distance with all the words in vocabulary, and return the 
        // topN nearest words and it's distance.
        void GetNearestTerms(const std::string& word
                            ,std::vector<float>& best_dist
                            ,std::vector<std::string>& best_words
                            ,uint32_t topN){
             best_dist.clear();
             best_words.clear();
             std::vector<float> vec;
             long long index;
             GetWordVector(word, index, vec);
             if(index == -1)
                 return;
             long long a, b, c;
             for(a = 0; a < words_num_; ++a){
               // not contains the input word
               if(index == a)
                   continue;
               float dist = 0.0;
               for(b = 0; b < vector_size_; ++b)   // square accumulation
               {
                   dist += vec[b] * M[b + a*vector_size_];
               }
               // select topN distance and words
               best_dist.resize(topN);
               best_words.resize(topN);
               for(b = 0; b < topN; ++b){
                    if (dist > best_dist[b]) {  
                        for (c = topN - 1; c > b; --c) { 
                            best_dist[c] = best_dist[c - 1];
                            best_words[c] = best_words[c - 1];
                        }
                        best_dist[b] = dist;
                        std::string str(&vocab[a * max_w]);
                        best_words[b] = str;
                        break;
                    }
               }
             }
        }

        // To calcuate the cosine between vector, if the dimension of the two vector is not equal
        // we think the cosine distance is zero.
        float CosineDistance(const std::vector<float>& vec1, const std::vector<float>& vec2){
            double dist = 0.0;
            if(vec1.size() != vec2.size())
                return 0.0;
            long long a;
            for(a = 0; a < vec1.size(); ++a )
                dist += vec1[a] * vec2[a];
            return dist;
        }

        // Generate a vector for given sentence, firstly segment the string into several
        // tokens, and get the word vector for every word if exist, then add up the vector.
        void SentenceVector(const std::string& sents, std::vector<float>& vec){
            vec.clear();
            if(sents.empty())
                return;
            std::vector<std::string> tokens;
            segWrapper_->segment(sents, tokens, false);
            vec.resize(vector_size_);
            for(uint32_t i = 0; i < vector_size_; ++i)
                vec[i] = 0.0;
            std::vector<float> tmpTokenVec(vector_size_, 0);
            long long index = -1;
            for (uint32_t i = 0; i < tokens.size(); ++i) {
                GetWordVector(tokens[i], index, tmpTokenVec);
                if(tmpTokenVec.empty())
                    continue;
                // TODO: optimization
                // Algorithm to create sentence vector
                // add up all the dimension to represent sentence vector
                for (uint32_t j = 0; j < vector_size_; ++j) {
                    vec[j] += tmpTokenVec[j];
                }
            }
        }
};



#endif // nlp/w2v/distance.hpp
