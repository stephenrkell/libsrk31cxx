CXXFLAGS += -std=c++0x -g  -I../include -fPIC
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
HDRS := $(wildcard *.hpp)

$(warning OBJS is $(OBJS))

libsrk31c++.a: $(OBJS) $(HDRS)
	ar r "$@" $(OBJS)

clean:
	rm -f libsrk31c++.{a,so} *.o
