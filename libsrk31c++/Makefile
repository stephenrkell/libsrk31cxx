CXXFLAGS += -std=c++0x -g -I../c++-fileno/include -I${HOME}/opt/include -L../c++-fileno
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
HDRS := $(wildcard *.hpp)

$(warning OBJS is $(OBJS))

libsrk31c++.a: $(OBJS) $(HDRS)
	ar r "$@" $(OBJS)

