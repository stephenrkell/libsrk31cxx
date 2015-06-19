CXXFLAGS += -std=c++0x -g  -I../include -fPIC
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

-include ../config.mk

default: libsrk31c++.a libsrk31c++.so

DEPS := $(patsubst %.cpp,.%.d,$(wildcard *.cpp))
$(DEPS): .%.d: %.cpp
	$(CXX) -MM $(CXXFLAGS) "$<" > "$@"

-include $(DEPS)

$(warning OBJS is $(OBJS))

libsrk31c++.a: $(OBJS)
	ar r "$@" $(OBJS)

libsrk31c++.so: $(OBJS)
	$(CXX) -shared -o "$@" $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f libsrk31c++.a libsrk31c++.so *.o
