
WORK_PTH:=../../../
INC:= $(WORK_PTH)include/
INC_JIEBA:=$(WORK_PTH)common/cppjieba/
INC_JIEBA_DEP:=$(WORK_PTH)common/cppjieba/deps


TARGET=rating

SOURCE=$(TARGET).cc  mongoose.c $(WORK_PTH)include/ml/linear/linear.cpp \
	   $(WORK_PTH)include/ml/linear/tron.cpp $(WORK_PTH)libs/linear/blas/blas.a

CFLAGS = -W -Wall -I ./ -pthread -g -O2 -DMONGOOSE_ENABLE_THREADS $(CFLAGS_EXTRA)
LDFLAGS = -lboost_system -lboost_filesystem  -lglog
CC=g++


all: $(TARGET) output

$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS) $(LDFLAGS) -I $(INC) -I $(INC_JIEBA) -I $(INC_JIEBA_DEP)

output: $(TARGET)
	@echo; echo "Copy executable files ..."
	#@mv -v $(TARGET) bin/

clean:
	rm -rf $(TARGET) *.ext *.o
