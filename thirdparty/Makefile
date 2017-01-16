all: googletest zeromq glfw zlib libpng

googletest: dist cmake
	$(MAKE) -C googletest

zeromq: dist
	$(MAKE) -C zeromq

glfw: dist cmake
	$(MAKE) -C glfw

glm: dist/include
	cp -r glm/include/glm dist/include

libpng:
	$(MAKE) -C libpng

zlib:
	$(MAKE) -C zlib

cmake:
	$(MAKE) -C cmake

dist:
	-mkdir dist

dist/include: dist
	-mkdir dist/include

clean:
	$(MAKE) -C googletest clean
	$(MAKE) -C zeromq clean
	$(MAKE) -C glfw clean
	$(MAKE) -C cmake clean
	$(MAKE) -C libpng clean
	$(MAKE) -C zlib clean
	rm -rf dist

.PHONY: clean all default googletest zeromq glfw glm cmake
