
INC:= ../include/
INC_JIEBA:=/work/SF1R_Proj/hobby/hearse/cppjieba/include
INC_JIEBA_DEP:=/work/SF1R_Proj/hobby/hearse/cppjieba/deps


TARGET=t_rating

SOURCE=$(TARGET).cc /work/SF1R_Proj/hobby/zeus/comment-rating/include/ml/linear/linear.cpp \
	   /work/SF1R_Proj/hobby/zeus/comment-rating/include/ml/linear/tron.cpp ../libs/linear/blas/blas.a

all: $(TARGET) output

$(TARGET): $(SOURCE)
	g++ -g -o $(TARGET) $(SOURCE) -I $(INC) -I $(INC_JIEBA) -I $(INC_JIEBA_DEP)  -lboost_system -lboost_filesystem

output: $(TARGET)
	@echo; echo "Copy executable files ..."
	@mv -v $(TARGET) bin/

clean:
	rm -rf bin/$(TARGET) *.ext *.o
