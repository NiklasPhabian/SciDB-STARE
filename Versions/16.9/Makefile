
.PHONY: tests echo

BOOST_LOCATION=/opt/scidb/19.3/3rdparty/boost/
BOOST_INCLUDE=/usr/include/boost169/
BOOST_LIB=${BOOST_LOCATION}/lib
SCIDB_SOURCE_DIR=/home/griessbaum/scidb/

HSTM_DIR=/home/griessbaum/STARE/


SRCS := $(wildcard *.c)
SRCS_CPP := $(filter-out tests.cpp, $(wildcard *.cpp))
OBJS := $(SRCS:.c=.o)
OBJS_CPP := $(SRCS_CPP:.cpp=.cpp.o)

#CFLAGS=-pedantic -W -Wextra -Wall -Wno-strict-aliasing -Wno-long-long -Wno-unused-parameter -fPIC -D__STDC_FORMAT_MACROS -Wno-system-headers -isystem -O2 -g -DNDEBUG -ggdb3  -D__STDC_LIMIT_MACROS
CFLAGS= -O2 -fPIC -std=gnu++11
INC=	-I. -I../adjacency \
	-I$(HSTM_DIR)/include \
	-DPROJECT_ROOT="\"$(SCIDB_SOURCE_DIR)\"" \
	-I"$(SCIDB_SOURCE_DIR)/include" -I"$(BOOST_INCLUDE)"

LIBS=	\
	-L"$(HSTM_DIR)/unix" \
	-L"$(SCIDB_SOURCE_DIR)/lib" \
	-shared -Wl,-soname,libSTARE.so -L. \
	-LSTARE \
	-lm

libs: $(OBJS) $(OBJS_CPP)
	@if test ! -d "$(SCIDB_SOURCE_DIR)"; then echo  "Error. Try:\n\nmake SCIDB_SOURCE_DIR=<PATH TO SCIDB TRUNK>"; exit 1; fi 
	$(CXX) $(CFLAGS) $(INC) -o libSTARE.so \
		$(OBJS_CPP) $(OBJS) $(LIBS)

tests: tests.x
	./tests.x

all: libs tests

tests.x: $(OBJS) $(OBJS_CPP) tests.cpp.o 
	$(CXX) -o $@ $^ \
	-O2 -fPIC -std=gnu++11 \
	$(INC) \
	-L$(HSTM_DIR)/unix \
	-L"$(SCIDB_SOURCE_DIR)/lib" \
	-L"$(BOOST_LIB)" \
	-L/home/mrilee/opt/lib \
	-lscidbclient \
	-lboost_thread \
	-lboost_system \
	-lhstm \
	-lm \
	-llog4cxx

#	-static-libstdc++ -static-libgcc \
#	-L/opt/rh/devtoolset-2/root/usr/lib/gcc/x86_64-redhat-linux/4.8.2 \
#	-lstdc++ -lsupc++ \


#	$(CXX) $(CFLAGS) $(INC) -o libccl.so \

# x: hstmIndex.cpp.o
# 	$(CXX) $(CFLAGS) $(INC) -o $@ $< \
# 	-L$(HSTM_DIR)/unix -lhstm

%.cpp.o: %.cpp 
	 $(CXX) $(CFLAGS) $(INC) -o $@ -c $<

# %.o: %.c
# 	gcc $(CFLAGS) -I. -c $<

clean:
	rm -f *.o *.so *.x

version:
	$(CXX) --version

echo: 
	@echo SRCS: $(SRCS)
	@echo OBJS: $(OBJS)
	@echo SRCS_CPP: $(SRCS_CPP)
	@echo OBJS_CPP: $(OBJS_CPP)
