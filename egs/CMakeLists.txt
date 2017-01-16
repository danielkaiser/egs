cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(egs VERSION 0.0.1)

add_subdirectory(core)
add_subdirectory(glip)
add_subdirectory(plugins)

install(DIRECTORY python DESTINATION egs/lib)

