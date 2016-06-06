
INC:= ../include/
INC_JIEBA:=../../common/cppjieba/
INC_JIEBA_DEP:=../../common/cppjieba/deps


TARGET=rating

SOURCE=$(TARGET).cc  mongoose.c ../include/ml/linear/linear.cpp \
	   ../include/ml/linear/tron.cpp ../libs/linear/blas/blas.a

CFLAGS = -W -Wall -pthread -g -O2 -DMONGOOSE_ENABLE_THREADS 
LDFLAGS = -lboost_system -lboost_filesystem -lglog
CC=g++

all: $(TARGET) output

$(TARGET): $(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE) $(CFLAGS) $(LDFLAGS) -I $(INC) -I $(INC_JIEBA) -I $(INC_JIEBA_DEP)

output: $(TARGET)
	@echo; echo "Copy executable files ..."
	#@mv -v $(TARGET) bin/

clean:
	rm -rf $(TARGET) *.ext *.o
