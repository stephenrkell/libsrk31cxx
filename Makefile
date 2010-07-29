default: lib

.PHONY: src
src:
	$(MAKE) -C src

.PHONY: clean
clean:
	$(MAKE) -C src clean

.PHONY: lib
lib: src
	mkdir -p lib && cd lib && ln -sf ../src/libsrk31c++.a .
