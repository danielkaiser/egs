cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(egs_examples VERSION 0.0.1)


add_executable(molecule_example_c molecule_example.c)
target_link_libraries(molecule_example_c egs util)
target_include_directories(molecule_example_c PRIVATE ${PROJECT_SOURCE_DIR}/../egs/core ${PROJECT_SOURCE_DIR}/../egs/plugins/molecule_plugin)
install(TARGETS molecule_example_c DESTINATION egs/bin/examples)

add_executable(molecule_example_cxx molecule_example.cxx)
target_link_libraries(molecule_example_cxx egs util)
target_include_directories(molecule_example_cxx PRIVATE ${PROJECT_SOURCE_DIR}/../egs/core ${PROJECT_SOURCE_DIR}/../egs/plugins/molecule_plugin)
set_property(TARGET molecule_example_cxx PROPERTY CXX_STANDARD 11)
set_property(TARGET molecule_example_cxx PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET molecule_example_cxx PROPERTY CXX_EXTENSIONS OFF)
install(TARGETS molecule_example_cxx DESTINATION egs/bin/examples)

install(FILES molecule_example.py DESTINATION egs/bin/examples)
install(FILES h2o.xyz DESTINATION egs/bin/examples)


add_executable(callback_example_c callback_example.c)
target_link_libraries(callback_example_c egs util callback_plugin)
target_include_directories(callback_example_c PRIVATE ${PROJECT_SOURCE_DIR}/../egs/core ${PROJECT_SOURCE_DIR}/../egs/plugins/callback_plugin)
install(TARGETS callback_example_c DESTINATION egs/bin/examples)

add_executable(callback_example_cxx callback_example.cxx)
target_link_libraries(callback_example_cxx egs util callback_plugin)
target_include_directories(callback_example_cxx PRIVATE ${PROJECT_SOURCE_DIR}/../egs/core ${PROJECT_SOURCE_DIR}/../egs/plugins/callback_plugin)
set_property(TARGET callback_example_cxx PROPERTY CXX_STANDARD 11)
set_property(TARGET callback_example_cxx PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET callback_example_cxx PROPERTY CXX_EXTENSIONS OFF)
install(TARGETS callback_example_cxx DESTINATION egs/bin/examples)

install(FILES callback_example.py DESTINATION egs/bin/examples)
