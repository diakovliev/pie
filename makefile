src_tree := $(CURDIR)
build_tree ?= ${src_tree}/_build

export CTEST_OUTPUT_ON_FAILURE=1
export PIE_VERSION:=$(shell git describe --dirty=*)

.PHONY: prepare-%

all: build
	make -C ${build_tree} test

build: prepare
	make -C ${build_tree} -j4

prepare: 
	mkdir -p ${build_tree}
	cd ${build_tree} && cmake -G"Eclipse CDT4 - Unix Makefiles" $(src_tree)

clean:
	rm -rf ${build_tree}
