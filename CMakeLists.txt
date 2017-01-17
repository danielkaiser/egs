cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

set(CMAKE_INSTALL_RPATH "@loader_path/../../lib/egs")

if (EGS_DEBUG)
	add_definitions(-DDEBUG=1)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

set(THIRDPARTY_BINARY_DIR "${CMAKE_SOURCE_DIR}/thirdparty/build/${CMAKE_BUILD_TYPE}/")
include(ThirdpartyHelper)
link_directories(${THIRDPARTY_INSTALL_PREFIX}/lib)

add_subdirectory(egs)
add_subdirectory(examples)

install(DIRECTORY ${THIRDPARTY_INSTALL_PREFIX}/lib/ DESTINATION egs/lib)
install(DIRECTORY ${THIRDPARTY_INSTALL_PREFIX}/include/ DESTINATION egs/include)

