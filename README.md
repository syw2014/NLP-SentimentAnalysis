# Sentiment Analysis System

## 1.Introduction
Sentiment Analysis system is a NLP-ML system,which was a supervised learning system.In the current version there are only two emotions(good and bad), 
when you give some abritrary text,the system will analyise the text, then output the label and it's membership score.As it's supervised learning problem
,so when to run this system you should prepare samples(in this system , I had used the comments with good , bad and medium of products as samples),then
choose methods to generate features and select/extract key features,after feature preparation , we should vectorize the samples that they can be used in
training model.Now there are two apps in this repository, sentiment anaylysis(http service api and web), sentance similarity computation based on word2vec
model in E-commerce.


## 2.Related Algorithms and models
    The algorithms and models used in this system show as list:
    - String Tokenize Algorithms(designed by ourself)
    - Feature selection/extraction, tf-idf, CHI-square,LDA(not used in this version)
    - Model, linear/svm, word2vec

## 3.HowTo
    This system was dependent on some other open source or libraries, before  test 
    or use modules in repository, you should check whether the compile environment 
    is right. The dependent libraries are:
    - jsoncpp, to parse json data
    - boost, filesystem, normal functions, unordered_map, string algorithms,etc
## 4.Evalution
## 5.TODO Lists
    - Model Train:
        1> Design sample scale module to make sure the sample is available for linear or svm training.
        2> Integrate training module.
        3> Add parameter setting module, like SVM/Logistic Regression and parameters selection,
           and this module should be flexiable as far as possible, need more test on different
           samples.

    - Predict with trained model:
        1> Add parameter setting module, in current the model use the default settings.
        2> Design other probability computation model.

    - System:
        1> Optimizate the whole system remove some unnecessary process.

    - Further Works:
        1> Use models on multi-classification.
