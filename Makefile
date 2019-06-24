.PHONY: tests echo

BOOST_LOCATION=/opt/scidb/19.3/3rdparty/boost/
BOOST_INCLUDE=/usr/include/boost169/
BOOST_LIB=${BOOST_LOCATION}/lib

SCIDB_SOURCE_DIR=/home/griessbaum/Downloads/scidb-19.3.1.3a5c8419/
STARE_BUILD=/home/griessbaum/STARE_build/
STARE_SRC=/home/griessbaum/STARE/


SRCS := $(wildcard *.c)
SRCS_CPP := $(filter-out tests.cpp, $(wildcard *.cpp))
OBJS := $(SRCS:.c=.o)
OBJS_CPP := $(SRCS_CPP:.cpp=.cpp.o)


CFLAGS= -O2 -fPIC -std=gnu++14
INC=    -I. -I../adjacency \
        -I$(STARE_SRC)/include \
        -DPROJECT_ROOT="\"$(SCIDB_SOURCE_DIR)\"" \
        -I"$(SCIDB_SOURCE_DIR)/include" -I"$(BOOST_INCLUDE)"

LIBS=   \
        -L"$(STARE_BUILD)/src" \
        -L"$(SCIDB_SOURCE_DIR)/lib" \
        -shared -Wl,-soname,libSTARE.so -L. \
        -lSTARE \
        -lm

libs: $(OBJS) $(OBJS_CPP)
	@if test ! -d "$(SCIDB_SOURCE_DIR)"; then echo  "Error. Try:\n\nmake SCIDB_SOURCE_DIR=<PATH TO SCIDB TRUNK>"; exit 1; fi
	$(CXX) $(CFLAGS) $(INC) -o libSTARE.so \
	$(OBJS_CPP) $(OBJS) $(LIBS)


%.cpp.o: %.cpp
	$(CXX) $(CFLAGS) $(INC) -o $@ -c $<

clean:
	rm -f *.o *.so *.x

version:
	$(CXX) --version

echo:
	@echo SRCS: $(SRCS)
	@echo OBJS: $(OBJS)
	@echo SRCS_CPP: $(SRCS_CPP)
	@echo OBJS_CPP: $(OBJS_CPP) 
