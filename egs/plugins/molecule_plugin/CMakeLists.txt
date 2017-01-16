cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(calculate_bonds VERSION 0.0.1)
set(PLUGIN_NAME molecule_plugin)

set(SOURCE_FILES
    calculate_bonds.c
)

set(HEADER_FILES
    molecule_plugin.h
)

set(SHADER_FILES
)

set(PYTHON_FILES
    molecule_plugin.py
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(${PROJECT_NAME} egs)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../../core)

install(TARGETS ${PROJECT_NAME} DESTINATION egs/lib/plugins/${PLUGIN_NAME})
install(FILES ${SHADER_FILES} DESTINATION egs/lib/plugins/${PLUGIN_NAME})
install(FILES ${PYTHON_FILES} DESTINATION egs/lib/plugins/${PLUGIN_NAME})
install(FILES ${HEADER_FILES} DESTINATION egs/include)
