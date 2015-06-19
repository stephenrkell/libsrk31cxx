default: lib

config.mk:
	$(dir $(lastword $(MAKEFILE_LIST)))/configure

include config.mk

.PHONY: src
src:
	$(MAKE) -C src

.PHONY: clean
clean:
	$(MAKE) -C src clean
	rm -f lib/*.a lib/*.so

.PHONY: lib
lib: src
	mkdir -p lib && cd lib && ln -sf ../src/libsrk31c++.a ../src/libsrk31c++.so .

install:
	install -m 0644 -D -t $(INSTALL_PREFIX)/include/srk31 include/srk31/*
	install -m 0644 -t $(INSTALL_PREFIX)/lib lib/*

