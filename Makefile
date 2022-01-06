
# common configuration

CC ?= gcc
CXX ?= g++

# Pass SIZE=1 to perform a size test instead of a speed test.
# We want to use either -O3 or -Os, but several libraries annoyingly
# override everything with -O2. We are in some cases manually
# editing configure and CMakeLists.txt files to get rid of them.
ifeq ($(SIZE), 1)
	OPTCONFIG := -Os -DBENCHMARK_SIZE_OPTIMIZED=1
	OBJECT_SIZES := 2
	RESULTS_ARG := size
	RESULTS_DOC := results-size
	RESULTS_EXTENDED_DOC := results-size-extended
else
	OPTCONFIG := -O3
	OBJECT_SIZES := 2 4
	RESULTS_ARG := speed
	RESULTS_DOC := results
	RESULTS_EXTENDED_DOC := results-extended
endif

# Compiler optimization flags (both are added to LDFLAGS.)
OPTFLAGS := $(OPTCONFIG) -flto -fno-fat-lto-objects -DNDEBUG
LDOPTFLAGS := -s -fuse-linker-plugin -fuse-ld=gold

# Resolved compiler flags
# We specify defaults of the most recent language standards. The individual
# libraries may override them (e.g. YAJL which specifies -std=c99)
CPPFLAGS := -Wall -fPIC -DPIC -Wno-error $(OPTFLAGS) -Isrc/common
CFLAGS := $(CPPFLAGS) -std=c11
CXXFLAGS := $(CPPFLAGS) -std=gnu++14 # we use anonymous structs
LDFLAGS  := $(CPPFLAGS) $(LDOPTFLAGS)

# Other run configurations
FILE_OBJECT_SIZES = 1 2 3 4 5
ITERATIONS = 7


# the default "all" target recurses back into the makefile to run
# everything twice, once optimized for speed and once optimized for
# size. the run-iterations target again recurses back into the
# makefile to run all tests repeatedly.

.PHONY: all
all: speed size

.PHONY: speed
speed:
	make fetch
	make clean-builds
	make build data run-iterations results

.PHONY: size
size:
	make fetch
	make clean-builds
	make SIZE=1 build data run-iterations results

.PHONY: run-iterations
run-iterations:
	bash -c "for i in {1..${ITERATIONS}}; do make run; done"


# global targets

.PHONY: fetch
fetch: fetch-mpack fetch-cmp fetch-msgpack fetch-rapidjson fetch-yajl fetch-jansson fetch-libbson fetch-binn fetch-udp-json fetch-ubj fetch-mongo-cxx

.PHONY: build
build: build-common build-hash build-mpack build-cmp build-msgpack build-rapidjson build-yajl build-jansson build-libbson build-binn build-udp-json build-ubj build-mongo-cxx

.PHONY: run
run: run-hash run-mpack run-cmp run-msgpack run-rapidjson run-yajl run-jansson run-libbson run-binn run-udp-json run-ubj run-mongo-cxx

.PHONY: data
data: data-mp data-json data-bson data-binn data-ubjson

.PHONY: results
results: $(RESULTS_DOC).md $(RESULTS_EXTENDED_DOC).md

results.csv: results

$(RESULTS_DOC).md: results.csv tools/results.py
	echo "" > $(RESULTS_DOC).md
	grep "^model name" /proc/cpuinfo | head -n 1 | sed 's/^.*\s*:/CPU model:/' | sed 's/$$/  /' >> $(RESULTS_DOC).md
	grep "^bogomips" /proc/cpuinfo   | head -n 1 | sed 's/^.*\s*:/Bogomips:/' >> $(RESULTS_DOC).md
	date >> $(RESULTS_DOC).md
	git rev-parse HEAD >> $(RESULTS_DOC).md
	tools/results.py $(RESULTS_ARG) >> $(RESULTS_DOC).md

$(RESULTS_EXTENDED_DOC).md: results.csv tools/results.py
	echo "" > $(RESULTS_EXTENDED_DOC).md
	grep "^model name" /proc/cpuinfo | head -n 1 | sed 's/^.*\s*:/CPU model:/' | sed 's/$$/  /' >> $(RESULTS_EXTENDED_DOC).md
	grep "^bogomips" /proc/cpuinfo   | head -n 1 | sed 's/^.*\s*:/Bogomips:/' >> $(RESULTS_EXTENDED_DOC).md
	date >> $(RESULTS_EXTENDED_DOC).md
	git rev-parse HEAD >> $(RESULTS_EXTENDED_DOC).md
	tools/results.py $(RESULTS_ARG) extended >> $(RESULTS_EXTENDED_DOC).md

# the html generators below use pandoc, but pandoc isn't
# available on ARM, so we don't generate them by default

$(RESULTS_DOC).html: $(RESULTS_DOC).md
	touch $(RESULTS_DOC).html
	pandoc --from markdown_github --to html -o $(RESULTS_DOC).html $(RESULTS_DOC).md
	sed -i '1s@^@<style>table {border-collapse: collapse;}</style>\n@' $(RESULTS_DOC).html
	sed -i '1s@^@<style>th, td {padding: 5px;}</style>\n@' $(RESULTS_DOC).html
	sed -i '1s@^@<style>table, th, td {border: 1px solid lightgray;}</style>\n@' $(RESULTS_DOC).html
	sed -i '1s@^@<!DOCTYPE html>\n<meta charset="UTF-8">\n@' $(RESULTS_DOC).html

$(RESULTS_EXTENDED_DOC).html: $(RESULTS_EXTENDED_DOC).md
	touch $(RESULTS_EXTENDED_DOC).html
	pandoc --from markdown_github --to html -o $(RESULTS_EXTENDED_DOC).html $(RESULTS_EXTENDED_DOC).md
	sed -i '1s@^@<style>table {border-collapse: collapse;}</style>\n@' $(RESULTS_EXTENDED_DOC).html
	sed -i '1s@^@<style>th, td {padding: 5px;}</style>\n@' $(RESULTS_EXTENDED_DOC).html
	sed -i '1s@^@<style>table, th, td {border: 1px solid lightgray;}</style>\n@' $(RESULTS_EXTENDED_DOC).html
	sed -i '1s@^@<!DOCTYPE html>\n<meta charset="UTF-8">\n@' $(RESULTS_EXTENDED_DOC).html

.PHONY: clean-builds
clean-builds:
	-rm -rf build
	-( cd $(msgpack-dir) && ( make clean ; make distclean ) )
	-( cd $(jansson-dir) && ( make clean ; make distclean ) )
	-( cd $(libbson-dir) && ( make clean ; make distclean ) )
	-( cd $(mongo-cxx-dir) && ( rm -r build ) )
	-rm -rf "$(yajl-dir)/build/"
	-rm -rf "$(ubj-dir)/build/"

.PHONY: clean
clean: clean-builds
	-rm -rf results*

.PHONY: distclean
distclean: clean
	-rm -rf contrib


# common

common-headers := src/common/generator.h src/common/benchmark.h
common-objs := build/common/generator.o build/common/benchmark.o
.PHONY: build-common
build-common: $(common-objs)

build/common/generator.o: $(common-headers) src/common/generator.c
	mkdir -p build/common
	$(CC) $(CFLAGS) -I contrib/mpack -c -o $@ src/common/generator.c

build/common/benchmark.o: $(common-headers) src/common/benchmark.c
	mkdir -p build/common
	$(CC) $(CFLAGS) -I contrib/mpack -c -o $@ src/common/benchmark.c



# hash benchmarks

.PHONY: run-hash
run-hash: run-hash-object run-hash-data

.PHONY: build-hash
build-hash: build/hash-object build/hash-data

# hash-object

build/hash/hash-object.o: $(common-headers) src/hash/hash-object.c
	mkdir -p build/hash
	$(CC) $(CFLAGS) -c -o $@ src/hash/hash-object.c

build/hash-object: build/hash/hash-object.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-hash-object
run-hash-object: build/hash-object
	build/hash-object $(OBJECT_SIZES)

# hash-data

build/hash/hash-data.o: $(common-headers) src/hash/hash-data.c
	mkdir -p build/hash
	$(CC) $(CFLAGS) -c -o $@ src/hash/hash-data.c

build/hash-data: build/hash/hash-data.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-hash-data
run-hash-data: build/hash-data
	build/hash-data $(OBJECT_SIZES)



# mpack

mpack-version := 0.8.1
mpack-url := https://github.com/ludocode/mpack/releases/download/v$(mpack-version)/mpack-amalgamation-$(mpack-version).tar.gz
mpack-dir := contrib/mpack/mpack-amalgamation-$(mpack-version)/src
mpack-config := $(mpack-dir)/mpack-config.h

.PHONY: fetch-mpack
fetch-mpack: $(mpack-config)
$(mpack-config):
	mkdir -p contrib/mpack
	cd contrib/mpack ;\
		curl -LO $(mpack-url) ;\
		tar -xzf mpack-amalgamation-$(mpack-version).tar.gz
	cp $(mpack-dir)/mpack-config.h.sample $(mpack-config)

build/mpack/mpack.o: $(mpack-config)
	mkdir -p build/mpack
	$(CC) $(CFLAGS) -I $(mpack-dir) -c -o $@ $(mpack-dir)/mpack/mpack.c

MPACK_TRACKING_FLAGS := -DMPACK_READ_TRACKING=1 -DMPACK_WRITE_TRACKING=1
build/mpack/mpack-tracking.o: $(mpack-config)
	mkdir -p build/mpack
	$(CC) $(CFLAGS) $(MPACK_TRACKING_FLAGS) -I $(mpack-dir) -c -o $@ $(mpack-dir)/mpack/mpack.c

.PHONY: run-mpack
run-mpack: run-mpack-write run-mpack-read run-mpack-node run-mpack-tracking-write run-mpack-tracking-read run-mpack-utf8-read run-mpack-utf8-node

.PHONY: build-mpack
build-mpack: build/mpack-file build/mpack-write build/mpack-read build/mpack-node build/mpack-tracking-write build/mpack-tracking-read build/mpack-utf8-read build/mpack-utf8-node

# mpack-file

build/mpack/mpack-file.o: $(common-headers) $(mpack-config) src/mpack/mpack-file.c
	mkdir -p build/mpack
	$(CC) $(CFLAGS) -I $(mpack-dir) -c -o $@ src/mpack/mpack-file.c

build/mpack-file: build/mpack/mpack.o build/mpack/mpack-file.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: data-mp
data-mp: run-mpack-file

.PHONY: run-mpack-file
run-mpack-file: build/mpack-file
	build/mpack-file $(FILE_OBJECT_SIZES)
build/data.mp: run-mpack-file

# mpack-write

build/mpack/mpack-write.o: $(common-headers) $(mpack-config) src/mpack/mpack-write.c
	mkdir -p build/mpack
	$(CC) $(CFLAGS) -I $(mpack-dir) -c -o $@ src/mpack/mpack-write.c

build/mpack-write: build/mpack/mpack.o build/mpack/mpack-write.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-mpack-write
run-mpack-write: build/mpack-write
	build/mpack-write $(OBJECT_SIZES)

# mpack-read

build/mpack/mpack-read.o: $(common-headers) $(mpack-config) src/mpack/mpack-read.c
	mkdir -p build/mpack
	$(CC) $(CFLAGS) -I $(mpack-dir) -c -o $@ src/mpack/mpack-read.c

build/mpack-read: build/mpack/mpack.o build/mpack/mpack-read.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-mpack-read
run-mpack-read: build/mpack-read data-mp
	build/mpack-read $(OBJECT_SIZES)

# mpack-node

build/mpack/mpack-node.o: $(common-headers) $(mpack-config) src/mpack/mpack-node.c
	mkdir -p build/mpack
	$(CC) $(CFLAGS) -I $(mpack-dir) -c -o $@ src/mpack/mpack-node.c

build/mpack-node: build/mpack/mpack.o build/mpack/mpack-node.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-mpack-node
run-mpack-node: build/mpack-node data-mp
	build/mpack-node $(OBJECT_SIZES)

# mpack-tracking-write

build/mpack/mpack-tracking-write.o: $(common-headers) $(mpack-config) src/mpack/mpack-write.c
	mkdir -p build/mpack
	$(CC) $(CFLAGS) $(MPACK_TRACKING_FLAGS) -I $(mpack-dir) -c -o $@ src/mpack/mpack-write.c

build/mpack-tracking-write: build/mpack/mpack-tracking.o build/mpack/mpack-tracking-write.o $(common-objs)
	$(CC) $(LDFLAGS) $(MPACK_TRACKING_FLAGS) -o $@ $^

.PHONY: run-mpack-tracking-write
run-mpack-tracking-write: build/mpack-tracking-write
	build/mpack-tracking-write $(OBJECT_SIZES)

# mpack-tracking-read

build/mpack/mpack-tracking-read.o: $(common-headers) $(mpack-config) src/mpack/mpack-read.c
	mkdir -p build/mpack
	$(CC) $(CFLAGS) $(MPACK_TRACKING_FLAGS) -I $(mpack-dir) -c -o $@ src/mpack/mpack-read.c

build/mpack-tracking-read: build/mpack/mpack-tracking.o build/mpack/mpack-tracking-read.o $(common-objs)
	$(CC) $(LDFLAGS) $(MPACK_TRACKING_FLAGS) -o $@ $^

.PHONY: run-mpack-tracking-read
run-mpack-tracking-read: build/mpack-tracking-read data-mp
	build/mpack-tracking-read $(OBJECT_SIZES)

# mpack-utf8-read

build/mpack/mpack-utf8-read.o: $(common-headers) $(mpack-config) src/mpack/mpack-read.c
	mkdir -p build/mpack
	$(CC) $(CFLAGS) -DCHECK_UTF8=1 -I $(mpack-dir) -c -o $@ src/mpack/mpack-read.c

build/mpack-utf8-read: build/mpack/mpack.o build/mpack/mpack-utf8-read.o $(common-objs)
	$(CC) $(LDFLAGS) -DCHECK_UTF8=1 -o $@ $^

.PHONY: run-mpack-utf8-read
run-mpack-utf8-read: build/mpack-utf8-read data-mp
	build/mpack-utf8-read $(OBJECT_SIZES)

# mpack-utf8-node

build/mpack/mpack-utf8-node.o: $(common-headers) $(mpack-config) src/mpack/mpack-node.c
	mkdir -p build/mpack
	$(CC) $(CFLAGS) -DCHECK_UTF8=1 -I $(mpack-dir) -c -o $@ src/mpack/mpack-node.c

build/mpack-utf8-node: build/mpack/mpack.o build/mpack/mpack-utf8-node.o $(common-objs)
	$(CC) $(LDFLAGS) -DCHECK_UTF8=1 -o $@ $^

.PHONY: run-mpack-utf8-node
run-mpack-utf8-node: build/mpack-utf8-node data-mp
	build/mpack-utf8-node $(OBJECT_SIZES)



# cmp

# as of this writing, cmp has not done a release since version 10, but we'll
# give them the benefit of the doubt and just use the current contents of the
# repository (since that is most likely how it is intended to be used.)
cmp-revision := daec9e09d7f7f79209b318f4a71a23981e3879be
cmp-url := https://github.com/camgunz/cmp/archive/$(cmp-revision).tar.gz
cmp-dir := contrib/cmp/cmp-$(cmp-revision)
cmp-header := $(cmp-dir)/cmp.h

.PHONY: fetch-cmp
fetch-cmp: $(cmp-header)
$(cmp-header):
	mkdir -p contrib/cmp
	cd contrib/cmp ;\
		curl -LO $(cmp-url) ;\
		tar -xzf $(cmp-revision).tar.gz

build/cmp/cmp.o: $(cmp-header)
	mkdir -p build/cmp
	$(CC) $(CFLAGS) -I $(cmp-dir) -c -o build/cmp/cmp.o $(cmp-dir)/cmp.c

.PHONY: run-cmp
run-cmp: run-cmp-write run-cmp-read

.PHONY: build-cmp
build-cmp: build/cmp-write build/cmp-read

# cmp-read

build/cmp/cmp-read.o: $(common-headers) $(cmp-header) src/cmp/cmp-read.c
	mkdir -p build/cmp
	$(CC) $(CFLAGS) -I $(cmp-dir) -c -o $@ src/cmp/cmp-read.c

build/cmp-read: build/cmp/cmp.o build/cmp/cmp-read.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-cmp-read
run-cmp-read: build/cmp-read data-mp
	build/cmp-read $(OBJECT_SIZES)

# cmp-write

build/cmp/cmp-write.o: $(common-headers) $(cmp-header) src/cmp/cmp-write.c
	mkdir -p build/cmp
	$(CC) $(CFLAGS) -I $(cmp-dir) -c -o $@ src/cmp/cmp-write.c

build/cmp-write: build/cmp/cmp.o build/cmp/cmp-write.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-cmp-write
run-cmp-write: build/cmp-write
	build/cmp-write $(OBJECT_SIZES)



# msgpack

msgpack-version := 1.3.0
msgpack-url := https://github.com/msgpack/msgpack-c/releases/download/cpp-$(msgpack-version)/msgpack-$(msgpack-version).tar.gz
msgpack-dir := contrib/msgpack/msgpack-$(msgpack-version)
msgpack-lib := $(msgpack-dir)/src/.libs/libmsgpack.a
msgpack-header := $(msgpack-dir)/include/msgpack.h

.PHONY: fetch-msgpack
fetch-msgpack: $(msgpack-header)
$(msgpack-header):
	mkdir -p contrib/msgpack
	cd contrib/msgpack ;\
		curl -LO $(msgpack-url) ;\
		tar -xzf msgpack-$(msgpack-version).tar.gz

$(msgpack-lib): $(msgpack-header)
	# libtool strips off -fuse-ld=gold, so we pass all our compiler flags
	# as part of CC and CXX. we also pass them as CFLAGS and CXXFLAGS to
	# prevent msgpack from overriding them.
	cd $(msgpack-dir); CC="$(CC) $(CFLAGS)" CXX="$(CXX) $(CXXFLAGS)" CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)" ./configure && make

.PHONY: run-msgpack
run-msgpack: run-msgpack-c-pack run-msgpack-cpp-pack run-msgpack-c-unpack run-msgpack-cpp-unpack

.PHONY: build-msgpack
build-msgpack: build/msgpack-c-pack build/msgpack-cpp-pack build/msgpack-c-unpack build/msgpack-cpp-unpack

# msgpack-c-unpack

build/msgpack/msgpack-c-unpack.o: $(common-headers) $(msgpack-lib) src/msgpack/msgpack-c-unpack.c
	mkdir -p build/msgpack
	$(CC) $(CFLAGS) -I $(msgpack-dir) -I $(msgpack-dir)/include -c -o $@ src/msgpack/msgpack-c-unpack.c

build/msgpack-c-unpack: build/msgpack/msgpack-c-unpack.o $(common-objs) $(msgpack-lib)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-msgpack-c-unpack
run-msgpack-c-unpack: build/msgpack-c-unpack data-mp
	build/msgpack-c-unpack $(OBJECT_SIZES)

# msgpack-cpp-unpack

build/msgpack/msgpack-cpp-unpack.o: $(common-headers) $(msgpack-lib) src/msgpack/msgpack-cpp-unpack.cpp
	mkdir -p build/msgpack
	$(CXX) $(CXXFLAGS) -I $(msgpack-dir) -I $(msgpack-dir)/include -c -o $@ src/msgpack/msgpack-cpp-unpack.cpp

build/msgpack-cpp-unpack: build/msgpack/msgpack-cpp-unpack.o $(common-objs) $(msgpack-lib)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: run-msgpack-cpp-unpack
run-msgpack-cpp-unpack: build/msgpack-cpp-unpack data-mp
	build/msgpack-cpp-unpack $(OBJECT_SIZES)

# msgpack-c-pack

build/msgpack/msgpack-c-pack.o: $(common-headers) $(msgpack-lib) src/msgpack/msgpack-c-pack.c
	mkdir -p build/msgpack
	$(CC) $(CFLAGS) -I $(msgpack-dir) -I $(msgpack-dir)/include -c -o $@ src/msgpack/msgpack-c-pack.c

build/msgpack-c-pack: build/msgpack/msgpack-c-pack.o $(common-objs) $(msgpack-lib)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-msgpack-c-pack
run-msgpack-c-pack: build/msgpack-c-pack
	build/msgpack-c-pack $(OBJECT_SIZES)

# msgpack-cpp-pack

build/msgpack/msgpack-cpp-pack.o: $(common-headers) $(msgpack-lib) src/msgpack/msgpack-cpp-pack.cpp
	mkdir -p build/msgpack
	$(CXX) $(CXXFLAGS) -I $(msgpack-dir) -I $(msgpack-dir)/include -c -o $@ src/msgpack/msgpack-cpp-pack.cpp

build/msgpack-cpp-pack: build/msgpack/msgpack-cpp-pack.o $(common-objs) $(msgpack-lib)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: run-msgpack-cpp-pack
run-msgpack-cpp-pack: build/msgpack-cpp-pack
	build/msgpack-cpp-pack $(OBJECT_SIZES)



# rapidjson

rapidjson-version := 1.0.2
rapidjson-url := https://github.com/miloyip/rapidjson/archive/v$(rapidjson-version).tar.gz
rapidjson-dir := contrib/rapidjson/rapidjson-$(rapidjson-version)
rapidjson-tarball := v$(rapidjson-version).tar.gz
rapidjson-header := $(rapidjson-dir)/include/rapidjson/rapidjson.h

.PHONY: fetch-rapidjson
fetch-rapidjson: $(rapidjson-header)
$(rapidjson-header):
	mkdir -p contrib/rapidjson
	cd contrib/rapidjson ;\
		curl -LO $(rapidjson-url) ;\
		tar -xzf $(rapidjson-tarball)

.PHONY: run-rapidjson
run-rapidjson: run-rapidjson-write run-rapidjson-sax run-rapidjson-insitu-sax run-rapidjson-dom run-rapidjson-insitu-dom

.PHONY: build-rapidjson
build-rapidjson: build/rapidjson-file build/rapidjson-write build/rapidjson-sax build/rapidjson-insitu-sax build/rapidjson-dom build/rapidjson-insitu-dom

# rapidjson-file

build/rapidjson/rapidjson-file.o: $(common-headers) $(rapidjson-header) src/rapidjson/rapidjson-file.cpp
	mkdir -p build/rapidjson
	$(CXX) $(CXXFLAGS) -I $(rapidjson-dir) -I $(rapidjson-dir)/include -c -o $@ src/rapidjson/rapidjson-file.cpp

build/rapidjson-file: build/rapidjson/rapidjson-file.o $(common-objs)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: data-json
data-json: run-rapidjson-file

.PHONY: run-rapidjson-file
run-rapidjson-file: build/rapidjson-file
	build/rapidjson-file $(FILE_OBJECT_SIZES)
build/data.json: run-rapidjson-file

# rapidjson-write

build/rapidjson/rapidjson-write.o: $(common-headers) $(rapidjson-header) src/rapidjson/rapidjson-write.cpp
	mkdir -p build/rapidjson
	$(CXX) $(CXXFLAGS) -I $(rapidjson-dir) -I $(rapidjson-dir)/include -c -o $@ src/rapidjson/rapidjson-write.cpp

build/rapidjson-write: build/rapidjson/rapidjson-write.o $(common-objs)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: run-rapidjson-write
run-rapidjson-write: build/rapidjson-write
	build/rapidjson-write $(OBJECT_SIZES)

# rapidjson-sax

build/rapidjson/rapidjson-sax.o: $(common-headers) $(rapidjson-header) src/rapidjson/rapidjson-sax.cpp
	mkdir -p build/rapidjson
	$(CXX) $(CXXFLAGS) -I $(rapidjson-dir) -I $(rapidjson-dir)/include -c -o $@ src/rapidjson/rapidjson-sax.cpp

build/rapidjson-sax: build/rapidjson/rapidjson-sax.o $(common-objs)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: run-rapidjson-sax
run-rapidjson-sax: build/rapidjson-sax data-json
	build/rapidjson-sax $(OBJECT_SIZES)

# rapidjson-insitu-sax

build/rapidjson/rapidjson-insitu-sax.o: $(common-headers) $(rapidjson-header) src/rapidjson/rapidjson-sax.cpp
	mkdir -p build/rapidjson
	$(CXX) $(CXXFLAGS) -DBENCHMARK_IN_SITU=1 -I $(rapidjson-dir) -I $(rapidjson-dir)/include -c -o $@ src/rapidjson/rapidjson-sax.cpp

build/rapidjson-insitu-sax: build/rapidjson/rapidjson-insitu-sax.o $(common-objs)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: run-rapidjson-insitu-sax
run-rapidjson-insitu-sax: build/rapidjson-insitu-sax data-json
	build/rapidjson-insitu-sax $(OBJECT_SIZES)

# rapidjson-dom

build/rapidjson/rapidjson-dom.o: $(common-headers) $(rapidjson-header) src/rapidjson/rapidjson-dom.cpp
	mkdir -p build/rapidjson
	$(CXX) $(CXXFLAGS) -I $(rapidjson-dir) -I $(rapidjson-dir)/include -c -o $@ src/rapidjson/rapidjson-dom.cpp

build/rapidjson-dom: build/rapidjson/rapidjson-dom.o $(common-objs)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: run-rapidjson-dom
run-rapidjson-dom: build/rapidjson-dom data-json
	build/rapidjson-dom $(OBJECT_SIZES)

# rapidjson-insitu-dom

build/rapidjson/rapidjson-insitu-dom.o: $(common-headers) $(rapidjson-header) src/rapidjson/rapidjson-dom.cpp
	mkdir -p build/rapidjson
	$(CXX) $(CXXFLAGS) -DBENCHMARK_IN_SITU=1 -I $(rapidjson-dir) -I $(rapidjson-dir)/include -c -o $@ src/rapidjson/rapidjson-dom.cpp

build/rapidjson-insitu-dom: build/rapidjson/rapidjson-insitu-dom.o $(common-objs)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: run-rapidjson-insitu-dom
run-rapidjson-insitu-dom: build/rapidjson-insitu-dom data-json
	build/rapidjson-insitu-dom $(OBJECT_SIZES)



# yajl

yajl-version := 2.1.0
yajl-url := http://github.com/lloyd/yajl/tarball/$(yajl-version)
yajl-tarball := yajl-$(yajl-version).tar.gz
yajl-dir := contrib/yajl/lloyd-yajl-66cb08c
yajl-include := $(yajl-dir)/build/yajl-$(yajl-version)/include
yajl-lib := $(yajl-dir)/build/yajl-$(yajl-version)/lib/libyajl_s.a
yajl-src := $(yajl-dir)/src/api/yajl_parse.h

.PHONY: fetch-yajl
fetch-yajl: $(yajl-src)
$(yajl-src):
	mkdir -p contrib/yajl
	cd contrib/yajl ;\
		curl -L -o $(yajl-tarball) $(yajl-url) ;\
		tar -xzf $(yajl-tarball)

$(yajl-lib): $(yajl-src)
	# We have to clear out YAJL's release flags since they override
	# ours (with -O2, which seems to be about half the speed of -O3)
	sed -i '/CMAKE_C_FLAGS_RELEASE/d' $(yajl-dir)/CMakeLists.txt
	
	# Due to a bug in CMake 3.4.0, we have to manually invoke cmake with
	# -DCMAKE_C_STANDARD_COMPUTED_DEFAULT=GNU in order to have -flto in
	# our CFLAGS. This will apparently be fixed in 3.4.0:
	#   http://public.kitware.com/pipermail/cmake-developers/2015-November/026993.html
	mkdir -p $(yajl-dir)/build
	cd $(yajl-dir)/build ;\
		CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" CPPFLAGS="" LDFLAGS="$(LDFLAGS)" \
			cmake .. \
			-DCMAKE_C_FLAGS_RELEASE="" \
			-DCMAKE_C_STANDARD_COMPUTED_DEFAULT=GNU \
			-DCMAKE_VERBOSE_MAKEFILE=1 \
			&& make

.PHONY: run-yajl
run-yajl: run-yajl-gen run-yajl-parse run-yajl-tree

.PHONY: build-yajl
build-yajl: build/yajl-gen build/yajl-parse build/yajl-tree

# yajl-gen

build/yajl/yajl-gen.o: $(common-headers) $(yajl-lib) src/yajl/yajl-gen.c
	mkdir -p build/yajl
	$(CC) $(CFLAGS) -I $(yajl-include) -c -o $@ src/yajl/yajl-gen.c

build/yajl-gen: build/yajl/yajl-gen.o $(common-objs) $(yajl-lib)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-yajl-gen
run-yajl-gen: build/yajl-gen
	build/yajl-gen $(OBJECT_SIZES)

# yajl-parse

build/yajl/yajl-parse.o: $(common-headers) $(yajl-lib) src/yajl/yajl-parse.c
	mkdir -p build/yajl
	$(CC) $(CFLAGS) -I $(yajl-include) -c -o $@ src/yajl/yajl-parse.c

build/yajl-parse: build/yajl/yajl-parse.o $(common-objs) $(yajl-lib)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-yajl-parse
run-yajl-parse: build/yajl-parse data-json
	build/yajl-parse $(OBJECT_SIZES)

# yajl-tree

build/yajl/yajl-tree.o: $(common-headers) $(yajl-lib) src/yajl/yajl-tree.c
	mkdir -p build/yajl
	$(CC) $(CFLAGS) -I $(yajl-include) -c -o $@ src/yajl/yajl-tree.c

build/yajl-tree: build/yajl/yajl-tree.o $(common-objs) $(yajl-lib)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-yajl-tree
run-yajl-tree: build/yajl-tree data-json
	build/yajl-tree $(OBJECT_SIZES)



# jansson

jansson-version := 2.7
jansson-url := http://www.digip.org/jansson/releases/jansson-$(jansson-version).tar.gz
jansson-dir := contrib/jansson/jansson-$(jansson-version)
jansson-include := $(jansson-dir)/src
jansson-lib := $(jansson-dir)/src/.libs/libjansson.a
jansson-header := $(jansson-dir)/src/jansson.h

.PHONY: fetch-jansson
fetch-jansson: $(jansson-header)
$(jansson-header):
	mkdir -p contrib/jansson
	cd contrib/jansson ;\
		curl -LO $(jansson-url) ;\
		tar -xzf jansson-$(jansson-version).tar.gz

$(jansson-lib): $(jansson-header)
	# jansson lets us specify our own flags! thank you for setting this up properly!
	cd $(jansson-dir); CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)" ./configure && make V=1

.PHONY: run-jansson
run-jansson: run-jansson-dump run-jansson-load run-jansson-ordered-dump run-jansson-ordered-load

.PHONY: build-jansson
build-jansson: build/jansson-dump build/jansson-load build/jansson-ordered-dump build/jansson-ordered-load

# jansson-dump

build/jansson/jansson-dump.o: $(common-headers) $(jansson-lib) src/jansson/jansson-dump.c
	mkdir -p build/jansson
	$(CC) $(CFLAGS) -I $(jansson-include) -c -o $@ src/jansson/jansson-dump.c

build/jansson-dump: build/jansson/jansson-dump.o $(common-objs) $(jansson-lib)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-jansson-dump
run-jansson-dump: build/jansson-dump
	build/jansson-dump $(OBJECT_SIZES)

# jansson-load

build/jansson/jansson-load.o: $(common-headers) $(jansson-lib) src/jansson/jansson-load.c
	mkdir -p build/jansson
	$(CC) $(CFLAGS) -I $(jansson-include) -c -o $@ src/jansson/jansson-load.c

build/jansson-load: build/jansson/jansson-load.o $(common-objs) $(jansson-lib)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-jansson-load
run-jansson-load: build/jansson-load data-json
	build/jansson-load $(OBJECT_SIZES)

# jansson-ordered-dump

build/jansson/jansson-ordered-dump.o: $(common-headers) $(jansson-lib) src/jansson/jansson-dump.c
	mkdir -p build/jansson
	$(CC) $(CFLAGS) -DPRESERVE_ORDER=1 -I $(jansson-include) -c -o $@ src/jansson/jansson-dump.c

build/jansson-ordered-dump: build/jansson/jansson-ordered-dump.o $(common-objs) $(jansson-lib)
	$(CC) $(LDFLAGS) -DPRESERVE_ORDER=1 -o $@ $^

.PHONY: run-jansson-ordered-dump
run-jansson-ordered-dump: build/jansson-ordered-dump
	build/jansson-ordered-dump $(OBJECT_SIZES)

# jansson-ordered-load

build/jansson/jansson-ordered-load.o: $(common-headers) $(jansson-lib) src/jansson/jansson-load.c
	mkdir -p build/jansson
	$(CC) $(CFLAGS) -DPRESERVE_ORDER=1 -I $(jansson-include) -c -o $@ src/jansson/jansson-load.c

build/jansson-ordered-load: build/jansson/jansson-ordered-load.o $(common-objs) $(jansson-lib)
	$(CC) $(LDFLAGS) -DPRESERVE_ORDER=1 -o $@ $^

.PHONY: run-jansson-ordered-load
run-jansson-ordered-load: build/jansson-ordered-load data-json
	build/jansson-ordered-load $(OBJECT_SIZES)



# libbson

libbson-version := 1.3.0
libbson-tarball := libbson-$(libbson-version).tar.gz
libbson-url := https://github.com/mongodb/libbson/releases/download/$(libbson-version)/$(libbson-tarball)
libbson-dir := contrib/libbson/libbson-$(libbson-version)
libbson-lib := $(libbson-dir)/.libs/libbson.a
libbson-src := $(libbson-dir)/src/bson/bson.h
libbson-include := $(libbson-dir)/src/bson

.PHONY: fetch-libbson
fetch-libbson: $(libbson-src)
$(libbson-src):
	mkdir -p contrib/libbson
	cd contrib/libbson ;\
		curl -L -O $(libbson-url) ;\
		tar -xzf $(libbson-tarball)

$(libbson-lib): $(libbson-src)
	# We have to clear out libbson's release flags since they override ours with -O2
	sed -i 's/-O2/ /g' $(libbson-dir)/configure
	mkdir -p $(libbson-dir)/build
	cd $(libbson-dir); CFLAGS=" $(CFLAGS) " CXXFLAGS=" $(CXXFLAGS) " CPPFLAGS=" " LDFLAGS=" $(LDFLAGS) " ./configure && make V=1 libbson.la

.PHONY: run-libbson
run-libbson: run-libbson-append run-libbson-iter

.PHONY: build-libbson
build-libbson: build/libbson-file build/libbson-append build/libbson-iter

# libbson requires pthreads, at least in debug mode (-flto seems
# to be able to eliminate this dependency, but we still want to
# be able to build without optimization for testing purposes.)
BSONLDFLAGS := $(LDFLAGS) -pthread

# libbson-file

build/libbson/libbson-file.o: $(common-headers) $(libbson-lib) src/libbson/libbson-file.c
	mkdir -p build/libbson
	$(CC) $(CFLAGS) -I $(libbson-include) -c -o $@ src/libbson/libbson-file.c

build/libbson-file: build/libbson/libbson-file.o $(common-objs) $(libbson-lib)
	$(CC) $(BSONLDFLAGS) -o $@ $^

.PHONY: data-bson
data-bson: run-libbson-file

.PHONY: run-libbson-file
run-libbson-file: build/libbson-file
	build/libbson-file $(FILE_OBJECT_SIZES)
build/data.bson: run-libbson-file

# libbson-append

build/libbson/libbson-append.o: $(common-headers) $(libbson-lib) $(libbson-config) src/libbson/libbson-append.c
	mkdir -p build/libbson
	$(CC) $(CFLAGS) -I $(libbson-include) -c -o $@ src/libbson/libbson-append.c

build/libbson-append: build/libbson/libbson-append.o $(common-objs) $(libbson-lib)
	$(CC) $(BSONLDFLAGS) -o $@ $^

.PHONY: run-libbson-append
run-libbson-append: build/libbson-append
	build/libbson-append $(OBJECT_SIZES)

# libbson-iter

build/libbson/libbson-iter.o: $(common-headers) $(libbson-lib) $(libbson-config) src/libbson/libbson-iter.c
	mkdir -p build/libbson
	$(CC) $(CFLAGS) -I $(libbson-include) -c -o $@ src/libbson/libbson-iter.c

build/libbson-iter: build/libbson/libbson-iter.o $(common-objs) $(libbson-lib)
	$(CC) $(BSONLDFLAGS) -o $@ $^

.PHONY: run-libbson-iter
run-libbson-iter: build/libbson-iter data-bson
	build/libbson-iter $(OBJECT_SIZES)



# binn

# binn has not done official releases yet. it also does
# not appear to have a version number.
binn-version := b3e2c27
binn-revision := b3e2c27c59747f2c79561fb0ee1c4228633d5581
binn-url := https://github.com/liteserver/binn/archive/$(binn-revision).tar.gz
binn-dir := contrib/binn/binn-$(binn-revision)/src
binn-header := $(binn-dir)/binn.h

.PHONY: fetch-binn
fetch-binn: $(binn-header)
$(binn-header):
	mkdir -p contrib/binn
	cd contrib/binn ;\
		curl -LO $(binn-url) ;\
		tar -xzf $(binn-revision).tar.gz ;\

# binn needs __BYTE_ORDER, so it needs sys/param.h (on RPi)
# also since binn does not publish a version number, we'll
# just insert the revision on the command line.
BINNFLAGS = $(CFLAGS) \
	-include sys/param.h \
	-DBENCHMARK_BINN_VERSION='"'$(binn-version)'"'

# binn suggests linking against the prebuilt library as a possible
# way to use it, but the makefile ignores CFLAGS and does not specify
# an optimization level. we'll just build it ourselves.
build/binn/binn.o: $(binn-header)
	mkdir -p build/binn
	$(CC) $(BINNFLAGS) -I $(binn-dir) -c -o build/binn/binn.o $(binn-dir)/binn.c

.PHONY: run-binn
run-binn: run-binn-write run-binn-load

.PHONY: build-binn
build-binn: build/binn-file build/binn-write build/binn-load

# binn-file

build/binn/binn-file.o: $(common-headers) $(binn-header) src/binn/binn-file.c
	mkdir -p build/binn
	$(CC) $(BINNFLAGS) -I $(binn-dir) -c -o $@ src/binn/binn-file.c

build/binn-file: build/binn/binn.o build/binn/binn-file.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: data-binn
data-binn: run-binn-file

.PHONY: run-binn-file
run-binn-file: build/binn-file
	build/binn-file $(FILE_OBJECT_SIZES)
build/data.mp: run-binn-file

# binn-write

build/binn/binn-write.o: $(common-headers) $(binn-header) src/binn/binn-write.c
	mkdir -p build/binn
	$(CC) $(BINNFLAGS) -I $(binn-dir) -c -o $@ src/binn/binn-write.c

build/binn-write: build/binn/binn.o build/binn/binn-write.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-binn-write
run-binn-write: build/binn-write
	build/binn-write $(OBJECT_SIZES)

# binn-load

build/binn/binn-load.o: $(common-headers) $(binn-header) src/binn/binn-load.c
	mkdir -p build/binn
	$(CC) $(BINNFLAGS) -I $(binn-dir) -c -o $@ src/binn/binn-load.c

build/binn-load: build/binn/binn.o build/binn/binn-load.o $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-binn-load
run-binn-load: build/binn-load data-binn
	build/binn-load $(OBJECT_SIZES)



# ubj

# ubj has not done official releases yet. it also does
# not appear to have a version number.
ubj-version := f4d85c4
ubj-revision := f4d85c4b848577d1f4d7b4437088609d2ee1fb08
ubj-url := https://github.com/Steve132/ubj/archive/$(ubj-revision).tar.gz
ubj-dir := contrib/ubj/ubj-$(ubj-revision)
ubj-header := $(ubj-dir)/ubj.h
ubj-lib := $(ubj-dir)/build/libubj.a

.PHONY: fetch-ubj
fetch-ubj: $(ubj-header)
$(ubj-header):
	mkdir -p contrib/ubj
	cd contrib/ubj ;\
		curl -LO $(ubj-url) ;\
		tar -xzf $(ubj-revision).tar.gz ;\

# ubj does not publish a version number, so we'll
# just insert the revision on the command line.
UBJFLAGS = $(CFLAGS) \
	-DBENCHMARK_UBJ_VERSION='"'$(ubj-version)'"'

$(ubj-lib): $(ubj-header)
	# Due to a bug in CMake 3.4.0, we have to manually invoke cmake with
	# -DCMAKE_C_STANDARD_COMPUTED_DEFAULT=GNU in order to have -flto in
	# our CFLAGS. This will apparently be fixed in 3.4.0:
	#   http://public.kitware.com/pipermail/cmake-developers/2015-November/026993.html
	mkdir -p $(ubj-dir)/build
	cd $(ubj-dir)/build ;\
		CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" CPPFLAGS="" LDFLAGS="$(LDFLAGS)" \
			cmake .. \
			-DCMAKE_C_FLAGS_RELEASE="" \
			-DCMAKE_C_STANDARD_COMPUTED_DEFAULT=GNU \
			-DCMAKE_VERBOSE_MAKEFILE=1 \
			&& make

.PHONY: run-ubj
run-ubj: run-ubj-write run-ubj-read run-ubj-opt-write run-ubj-opt-read

.PHONY: build-ubj
build-ubj: build/ubj-file build/ubj-write build/ubj-read build/ubj-opt-write build/ubj-opt-read

# ubj-file

build/ubj/ubj-file.o: $(common-headers) $(ubj-header) src/ubj/ubj-file.c
	mkdir -p build/ubj
	$(CC) $(UBJFLAGS) -I $(ubj-dir) -c -o $@ src/ubj/ubj-file.c

build/ubj-file: build/ubj/ubj-file.o $(ubj-lib) $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: data-ubjson
data-ubjson: run-ubj-file

.PHONY: run-ubj-file
run-ubj-file: build/ubj-file
	build/ubj-file $(FILE_OBJECT_SIZES)
build/data.ubjson: run-ubj-file

# ubj-write

build/ubj/ubj-write.o: $(common-headers) $(ubj-header) src/ubj/ubj-write.c
	mkdir -p build/ubj
	$(CC) $(UBJFLAGS) -I $(ubj-dir) -c -o $@ src/ubj/ubj-write.c

build/ubj-write: build/ubj/ubj-write.o $(ubj-lib) $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-ubj-write
run-ubj-write: build/ubj-write
	build/ubj-write $(OBJECT_SIZES)

# ubj-read

build/ubj/ubj-read.o: $(common-headers) $(ubj-header) src/ubj/ubj-read.c
	mkdir -p build/ubj
	$(CC) $(UBJFLAGS) -I $(ubj-dir) -c -o $@ src/ubj/ubj-read.c

build/ubj-read: build/ubj/ubj-read.o $(ubj-lib) $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-ubj-read
run-ubj-read: build/ubj-read data-ubjson
	build/ubj-read $(OBJECT_SIZES)

# ubj-opt-write

build/ubj/ubj-opt-write.o: $(common-headers) $(ubj-header) src/ubj/ubj-write.c
	mkdir -p build/ubj
	$(CC) $(UBJFLAGS) -DBENCHMARK_UBJ_OPTIMIZED=1 -I $(ubj-dir) -c -o $@ src/ubj/ubj-write.c

build/ubj-opt-write: build/ubj/ubj-opt-write.o $(ubj-lib) $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-ubj-opt-write
run-ubj-opt-write: build/ubj-opt-write
	build/ubj-opt-write $(OBJECT_SIZES)

# ubj-opt-read

build/ubj/ubj-opt-read.o: $(common-headers) $(ubj-header) src/ubj/ubj-read.c
	mkdir -p build/ubj
	$(CC) $(UBJFLAGS) -DBENCHMARK_UBJ_OPTIMIZED=1 -I $(ubj-dir) -c -o $@ src/ubj/ubj-read.c

build/ubj-opt-read: build/ubj/ubj-opt-read.o $(ubj-lib) $(common-objs)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run-ubj-opt-read
run-ubj-opt-read: build/ubj-opt-read data-ubjson
	build/ubj-opt-read $(OBJECT_SIZES)



# udp/json-parser and udp/json-builder
#
# this parser and builder are complementary; they are meant to be used
# together, and the builder requires the parser.
#
# the parser has two releases but the builder has none. instead, like
# CMP we'll give them the benefit of the doubt and just checkout the
# current source for both (there doesn't appear to be performance-related
# changes since json-parser's last release anyway.)
#
# these provide configure scripts to build libraries, but they are single
# .c files and they recommend just dropping the files into your project,
# so we'll ignore their build systems and just compile them ourselves.

.PHONY: fetch-udp-json
fetch-udp-json: fetch-json-parser fetch-json-builder

.PHONY: build-udp-json
build-udp-json: build-json-parser build-json-builder

.PHONY: run-udp-json
run-udp-json: run-json-parser run-json-builder

# json-parser

json-parser-version := 7053321
json-parser-revision := 70533215eea575e40a0b91a34ae01a779641d73a
json-parser-url := https://github.com/udp/json-parser/archive/$(json-parser-revision).tar.gz
json-parser-dir := contrib/udp-json/json-parser-$(json-parser-revision)
json-parser-header := $(json-parser-dir)/json.h

.PHONY: fetch-json-parser
fetch-json-parser: $(json-parser-header)
$(json-parser-header):
	mkdir -p contrib/udp-json
	cd contrib/udp-json ;\
		curl -LO $(json-parser-url) ;\
		tar -xzf $(json-parser-revision).tar.gz

.PHONY: build-json-parser
build-json-parser: build/json-parser

build/udp-json/json-lib.o: $(json-parser-header)
	mkdir -p build/udp-json
	$(CC) $(CFLAGS) -I $(json-parser-dir) -c -o $@ $(json-parser-dir)/json.c

build/udp-json/json-parser-test.o: $(common-headers) $(json-parser-header) src/udp-json/json-parser.c
	mkdir -p build/udp-json
	$(CC) $(CFLAGS) \
	-DBENCHMARK_JSON_PARSER_VERSION='"'$(json-parser-version)'"' \
	-I $(json-parser-dir) -c -o $@ src/udp-json/json-parser.c

build/json-parser: build/udp-json/json-lib.o build/udp-json/json-parser-test.o $(common-objs)
	$(CC) -lm $(LDFLAGS) -o $@ $^

.PHONY: run-json-parser
run-json-parser: build/json-parser data-json
	build/json-parser $(OBJECT_SIZES)

# json-builder

json-builder-version := 19c739f
json-builder-revision := 19c739f64d1da157789c35a06911b865486e6c2e
json-builder-url := https://github.com/udp/json-builder/archive/$(json-builder-revision).tar.gz
json-builder-dir := contrib/udp-json/json-builder-$(json-builder-revision)
json-builder-header := $(json-builder-dir)/json-builder.h

.PHONY: fetch-json-builder
fetch-json-builder: $(json-builder-header) $(json-parser-header)
$(json-builder-header):
	mkdir -p contrib/udp-json
	cd contrib/udp-json ;\
		curl -LO $(json-builder-url) ;\
		tar -xzf $(json-builder-revision).tar.gz

.PHONY: build-json-builder
build-json-builder: build/json-builder

build/udp-json/json-builder-lib.o: $(json-parser-header) $(json-builder-header)
	mkdir -p build/udp-json
	$(CC) $(CFLAGS) -I $(json-parser-dir) -I $(json-builder-dir) -c -o $@ $(json-builder-dir)/json-builder.c

build/udp-json/json-builder-test.o: $(common-headers) $(json-parser-header) $(json-builder-header) src/udp-json/json-builder.c
	mkdir -p build/udp-json
	$(CC) $(CFLAGS) \
	-DBENCHMARK_JSON_BUILDER_VERSION='"'$(json-builder-version)'"' \
	-I $(json-parser-dir) -I $(json-builder-dir) -c -o $@ src/udp-json/json-builder.c

build/json-builder: build/udp-json/json-lib.o build/udp-json/json-builder-lib.o build/udp-json/json-builder-test.o $(common-objs)
	$(CC) -lm $(LDFLAGS) -o $@ $^

.PHONY: run-json-builder
run-json-builder: build/json-builder
	build/json-builder $(OBJECT_SIZES)



# mongo-cxx

# the bson website recommends using just the bson portion of the
# MongoDB C++ Legacy Driver as the primary bson implementation
# for C++.
mongo-cxx-version := 1.1.0
mongo-cxx-url := https://github.com/mongodb/mongo-cxx-driver/archive/legacy-$(mongo-cxx-version).tar.gz
mongo-cxx-dir := contrib/mongo-cxx/mongo-cxx-driver-legacy-$(mongo-cxx-version)
mongo-cxx-lib := $(mongo-cxx-dir)/build/install/lib/libmongoclient.a
mongo-cxx-header := $(mongo-cxx-dir)/src/mongo/bson/bson.h

.PHONY: fetch-mongo-cxx
fetch-mongo-cxx: $(mongo-cxx-header)
$(mongo-cxx-header):
	mkdir -p contrib/mongo-cxx
	cd contrib/mongo-cxx ;\
		curl -LO $(mongo-cxx-url) ;\
		tar -xzf legacy-$(mongo-cxx-version).tar.gz

MONGOFLAGS = -Wno-deprecated-declarations

$(mongo-cxx-lib): $(mongo-cxx-header)
	# scons doesn't listen to environment flags, so we pass flags
	# as part of CC and CXX. the buildsystem puts the entire set
	# of optimizations into the build path but it still works.
	cd $(mongo-cxx-dir); scons --disable-warnings-as-errors --cc="$(CC) $(CFLAGS) $(MONGOFLAGS)" --cxx="$(CXX) $(CXXFLAGS) $(MONGOFLAGS)" install

.PHONY: run-mongo-cxx
run-mongo-cxx: run-mongo-cxx-obj run-mongo-cxx-builder

.PHONY: build-mongo-cxx
build-mongo-cxx: $(mongo-cxx-lib) build/mongo-cxx-obj build/mongo-cxx-builder

# mongo-cxx-builder

build/mongo-cxx/mongo-cxx-builder.o: $(common-headers) $(mongo-cxx-lib) src/mongo-cxx/mongo-cxx-builder.cpp
	mkdir -p build/mongo-cxx
	$(CXX) $(CXXFLAGS) $(MONGOFLAGS) -I $(mongo-cxx-dir) -I $(mongo-cxx-dir)/build/install/include -c -o $@ src/mongo-cxx/mongo-cxx-builder.cpp

build/mongo-cxx-builder: build/mongo-cxx/mongo-cxx-builder.o $(common-objs) $(mongo-cxx-lib)
	$(CXX) $(LDFLAGS) $(MONGOFLAGS) -lboost_system -lboost_thread -o $@ $^

.PHONY: run-mongo-cxx-builder
run-mongo-cxx-builder: build/mongo-cxx-builder
	build/mongo-cxx-builder $(OBJECT_SIZES)

# mongo-cxx-obj

build/mongo-cxx/mongo-cxx-obj.o: $(common-headers) $(mongo-cxx-lib) src/mongo-cxx/mongo-cxx-obj.cpp
	mkdir -p build/mongo-cxx
	$(CXX) $(CXXFLAGS) $(MONGOFLAGS) -I $(mongo-cxx-dir) -I $(mongo-cxx-dir)/build/install/include -c -o $@ src/mongo-cxx/mongo-cxx-obj.cpp

build/mongo-cxx-obj: build/mongo-cxx/mongo-cxx-obj.o $(common-objs) $(mongo-cxx-lib)
	$(CXX) $(LDFLAGS) $(MONGOFLAGS) -lboost_system -lboost_thread -o $@ $^

.PHONY: run-mongo-cxx-obj
run-mongo-cxx-obj: build/mongo-cxx-obj data-bson
	build/mongo-cxx-obj $(OBJECT_SIZES)

