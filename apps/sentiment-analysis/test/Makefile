
WORK_PTH:=../../../
INC:= $(WORK_PTH)include/
INC_JIEBA:=$(WORK_PTH)common/cppjieba/
INC_JIEBA_DEP:=$(WORK_PTH)common/cppjieba/deps


TARGET=t_rating

SOURCE=$(TARGET).cc $(WORK_PTH)include/ml/linear/linear.cpp \
	   $(WORK_PTH)include/ml/linear/tron.cpp $(WORK_PTH)libs/linear/blas/blas.a

all: $(TARGET) output

$(TARGET): $(SOURCE)
	g++ -g -o $(TARGET) $(SOURCE) -I $(INC) -I $(INC_JIEBA) -I $(INC_JIEBA_DEP)  -lboost_system -lboost_filesystem

output: $(TARGET)
	@echo; echo "Copy executable files ..."
	@mv -v $(TARGET) bin/

clean:
	rm -rf bin/$(TARGET) *.ext *.o
