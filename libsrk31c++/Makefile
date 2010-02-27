CXXFLAGS += -I ../c++-fileno/include -L../c++-fileno
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
HDRS := $(wildcard *.hpp)

$(warning OBJS is $(OBJS))

libsrk31c++.a: $(OBJS) $(HDRS)
	ar r "$@" $(OBJS)

