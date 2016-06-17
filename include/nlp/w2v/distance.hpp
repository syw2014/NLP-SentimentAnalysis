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
#include "nlp/segment/segmentWrapper.hpp"

// C headers
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

const long long max_size = 2000; // max length of strings
const long long N = 40;          // number of closest words
const long long max_w = 50;      // max length of vocabulary entries

class WordVectorPredict
{
    private:
        std::string model_;      // Trained model file name
        long long words_num_;    // Total words number in trained model, also is the size of vocabulary
        long long vector_size_;  // Word vector size, set when training model
       // long long word_index_;   // Store word index in word dictionary
        float *M;                // Memory for all  elemnets, it's size = vocabulary size * dimension * type size(float)              
        char *vocab;             // The memory for all terms, it's size = vocabulary size * term length * type size(char)
        
        
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
            
            vocab = (char *)malloc((long long)words_num_ * max_w * sizeof(char)); // Alloc memory for all terms
            if(vocab == NULL){
                std::cout << "Alloc memory: "<< (long long)words_num_ * max_w * sizeof(char) / 1048576 
                            <<"MB for terms failed!\n";
                return false;
            }
            M = (float *)malloc((long long)words * (long long)vector_size_ * sizeof(float)); // Alloc memory for all elements
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
                std::size_t len = 0;
                // Compute the square for every dimension and calcuate the sum for cosine compuataion
                // Trick,pre-process for cosine
                for(a = 0; a < vector_size_; ++a)
                    len += M[a + b * vector_size_] * M[a + b*vector_size_];
                len = sqrt(len);
                for(a = 0; a < vector_size_; ++a)
                    M[a + b*size] /= len;
            }
            fclose(ifs);
        }

    public:
        WordVectorPredict(){
        }
        ~WordVectorPredict(){
        }
        
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
           for(a = 0; a < vector_size_; ++a)
               vec[a] += M[a + b*size];
           long long elem_square_sum = 0;
           for(a = 0; a < vector_size_; ++a)
               elem_square_sum += vec[a] * vec[a];
           elem_square_sum = sqrt(elem_square_sum);
           // Trick to accelerate cosine computation
           // cosine = sum(xi*yi)/(|X||Y|)
           // cosine = sum (xi/|X|)*(yi/|Y|)
           for(a = 0; a < vector_size_; ++a)
               vec[a] /= elem_square_sum;
        }


        float Similarity(){
        }

        // To calcuate cosine distance with all the words in vocabulary, and return the 
        // topN nearest words.
        void GetNearestTerms(const std::string& word, uint32_t topN){
             std::vector<float> vec;
             long long index, a;
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
                   dist += vec[a] * M[b + a*size];
               // choose topN
               for(b = 0; b < topN; ++b){
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

        void SentenceVector(){
        }

        // Get the index of word in word dictionary
        //void GetWordIndex(){
        //    return word_index_;
        //}

};



#endif // nlp/w2v/distance.hpp
