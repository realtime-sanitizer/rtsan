PWD := $(shell pwd)

ifeq ($(RADSAN_BUILD_DIR),)
	BUILD_DIR := $(PWD)/build
else
	BUILD_DIR := $(RADSAN_BUILD_DIR)
endif

NPROCS := 1
OS := $(shell uname -s)
ARCH := $(shell uname -m)

ifeq ($(OS),Linux)
	NPROCS := $(shell grep -c ^processor /proc/cpuinfo)
endif
ifeq ($(OS),Darwin)
	NPROCS := $(shell sysctl -n hw.ncpu)
endif

CCACHE := $(shell which ccache)

clang: generate
	cmake --build $(BUILD_DIR) --target clang compiler-rt llvm-symbolizer -j$(NPROCS)
	echo "\nclang output:\n" && find $(BUILD_DIR)/bin -type l | grep clang;

build-folder:
	if [ ! -d $(BUILD_DIR) ]; then \
		mkdir -p $(BUILD_DIR); \
	fi

submodules:
	if [ ! -f llvm-project/README.md ]; then \
		git submodule update --init --recursive; \
	fi

generate: build-folder submodules
	if [ ! -f $(BUILD_DIR)/CMakeCache.txt ]; then \
		cmake -G "$${RADSAN_CMAKE_GENERATOR:-Unix Makefiles}" \
		-DCMAKE_BUILD_TYPE=Release \
		-DBUILD_SHARED_LIBS=ON \
		-DCOMPILER_RT_BUILD_SANITIZERS=ON \
		-DLLVM_ENABLE_PROJECTS="clang;compiler-rt" \
		-DLLVM_TARGETS_TO_BUILD=Native \
		-DCMAKE_C_COMPILER_LAUNCHER=$(CCACHE) \
		-DCMAKE_CXX_COMPILER_LAUNCHER=$(CCACHE) \
		-DSANITIZER_MIN_OSX_VERSION=$${RADSAN_MIN_OSX_VERSION:-10.15} \
		-B $(BUILD_DIR) \
		./llvm-project/llvm; \
	fi

test: generate 
	cmake --build $(BUILD_DIR) --target TRadsan-$(ARCH)-NoInstTest TRadsan-$(ARCH)-Test -j$(NPROCS)
	$(BUILD_DIR)/projects/compiler-rt/lib/radsan/tests/Radsan-$(ARCH)-NoInstTest
	$(BUILD_DIR)/projects/compiler-rt/lib/radsan/tests/Radsan-$(ARCH)-Test

check-compiler-rt: generate
	cmake --build $(BUILD_DIR) --target check-compiler-rt -j$(NPROCS)

docker:
	docker build -t radsan -f $(PWD)/docker/Dockerfile .

clean:
	rm -rf $(BUILD_DIR)

.PHONY: help clang build-folder submodules generate check-compiler-rt test clean docker test-all

help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  help              Show this help message"
	@echo "  generate          Generate build files"
	@echo "  clang             Build clang and compiler-rt"
	@echo "  check-compiler-rt Run all compiler-rt tests (extremely slow)"
	@echo "  test              Build and run radsan tests"
	@echo "  docker            Build docker image"
	@echo "  clean             Clean build directory"
	@echo ""
	@echo "Variables:"
	@echo "  RADSAN_BUILD_DIR        Build directory (default: build)"
	@echo "  RADSAN_CMAKE_GENERATOR  CMake generator (default: Unix Makefiles)"
	@echo "  RADSAN_MIN_OSX_VERSION  Minimum macOS version (default: 10.15)"
	@echo ""
	@echo "Example:"
	@echo "  make RADSAN_BUILD_DIR=build-clang RADSAN_CMAKE_GENERATOR=Ninja RADSAN_MIN_OSX_VERSION=10.14 clang"
