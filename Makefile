SHELL := /bin/bash
CMAKE ?= cmake
REQUIRED_CMAKE_VERSION="3.1.3"
DEFAULT_CMAKE := $(shell which $(CMAKE))
DEFAULT_CMAKE_VERSION := $(shell test "$(DEFAULT_CMAKE)" == "" || $(DEFAULT_CMAKE) --version | head -n 1)
THIRDPARTY_CMAKE := $(shell pwd)/thirdparty/cmake/cmake-src/bin/cmake
THIRDPARTY_CMAKE_URL := "https://github.com/Kitware/CMake/archive/v3.7.1.tar.gz"
# This will fail for CMake 3.1x due to string comparison being used
CMAKE := $(shell \
if [[ ! "cmake version $(REQUIRED_CMAKE_VERSION)" > "$(DEFAULT_CMAKE_VERSION)" ]]; then \
	which $(CMAKE); \
else \
	echo $(THIRDPARTY_CMAKE); \
fi)
EXTRA_CMAKE_FLAGS ?=
EGS_INSTALL_PREFIX ?= /usr/local

all: build/Makefile
	cd build && $(MAKE)

build/Makefile: $(CMAKE)
	mkdir -p build
	cd build && $(CMAKE) .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=$(EGS_INSTALL_PREFIX) $(EXTRA_CMAKE_FLAGS)

install: build/Makefile
	cd build && $(MAKE) install

debug: EXTRA_CMAKE_FLAGS += -DEGS_DEBUG=ON -DGLIP_DEBUG=ON
debug: all install

thirdparty/cmake/cmake-src.tar.gz:
	@if [ "$(DEFAULT_CMAKE)" == "" ]; then \
		echo "No CMake installed."; \
	else \
		echo "CMake $(REQUIRED_CMAKE_VERSION) required, but $(DEFAULT_CMAKE) is $(DEFAULT_CMAKE_VERSION)"; \
	fi
	@echo "Downloading CMake source from $(THIRDPARTY_CMAKE_URL)..."
	@curl -L -# --create-dirs -o thirdparty/cmake/cmake-src.tar.gz $(THIRDPARTY_CMAKE_URL)

thirdparty/cmake/cmake-src/bootstrap: thirdparty/cmake/cmake-src.tar.gz
	@echo "Extracting CMake source..."
	@tar xzm -C thirdparty/cmake -f thirdparty/cmake/cmake-src.tar.gz
	@mv -f thirdparty/cmake/`tar tf thirdparty/cmake/cmake-src.tar.gz | head -n 1 | cut -d '/' -f 1` thirdparty/cmake/cmake-src

$(THIRDPARTY_CMAKE): thirdparty/cmake/cmake-src/bootstrap
	@echo "Building CMake..."
	cd thirdparty/cmake/cmake-src && ./bootstrap --system-curl
	cd thirdparty/cmake/cmake-src && $(MAKE)

clean:
	rm -rf thirdparty/cmake
	rm -rf thirdparty/build
	rm -rf build
	rm -rf cmake-build-debug

.PHONY: clean debug all

