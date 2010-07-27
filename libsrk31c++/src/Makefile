CXXFLAGS += -std=c++0x -g -I../libc++fileno/include -I../include
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
HDRS := $(wildcard *.hpp)

$(warning OBJS is $(OBJS))

libsrk31c++.a: $(OBJS) $(HDRS)
	ar r "$@" $(OBJS)

