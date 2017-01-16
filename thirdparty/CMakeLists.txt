cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)


include(ExternalProject)
externalproject_add(glfw
        URL https://github.com/glfw/glfw/archive/3.2.1.tar.gz
        URL_HASH MD5=91b8250b6edcc26c9f5205555070a504
        PREFIX "${CMAKE_BINARY_DIR}/thirdparty/"
        CMAKE_ARGS "-DBUILD_SHARED_LIBS=ON;-DGLFW_BUILD_EXAMPLES=OFF;-DGLFW_BUILD_TESTS=OFF;-DGLFW_BUILD_DOCS=OFF;-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
)

externalproject_add(zeromq
        URL https://github.com/zeromq/libzmq/archive/v4.2.0.tar.gz
        URL_HASH MD5=189c31bf116b4fe929795a76bac7208a
        PREFIX "${CMAKE_BINARY_DIR}/thirdparty/"
        CMAKE_ARGS "-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=1;-DCMAKE_MACOSX_RPATH=ON;-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE};-DBUILD_SHARED_LIBS=1;-DZMQ_BUILD_TESTS=OFF;-DZMQ_BUILD_FRAMEWORK=OFF"
)

externalproject_add(zlib
        URL http://zlib.net/zlib-1.2.11.tar.gz
        URL_HASH MD5=1c9f62f0778697a09d36121ead88e08e
        PREFIX "${CMAKE_BINARY_DIR}/thirdparty/"
        CMAKE_ARGS "${zlib_CMAKE_ARGS};-DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX};-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DBUILD_SHARED_LIBS=OFF;-DCMAKE_MACOSX_RPATH=ON;-DCMAKE_POSITION_INDEPENDENT_CODE=ON;"
        )

externalproject_add(libpng
        URL http://download.sourceforge.net/libpng/libpng-1.6.28.tar.gz
        URL_HASH MD5=897ccec1ebfb0922e83c2bfaa1be8748
        DEPENDS zlib
        CMAKE_ARGS "${libpng_CMAKE_ARGS};-DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX};-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DPNG_SHARED=OFF;-DPNG_STATIC=ON;-DCMAKE_POSITION_INDEPENDENT_CODE=ON;"
        )


externalproject_add(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        PREFIX "${CMAKE_BINARY_DIR}/thirdparty/"
        CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        )


externalproject_add(glm
        URL https://github.com/g-truc/glm/archive/0.9.8.1.tar.gz
        URL_HASH MD5=6ef53481c48a2cb13d7ebca6245139e8
        PREFIX "${CMAKE_BINARY_DIR}/thirdparty/"
        CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX};-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        )
