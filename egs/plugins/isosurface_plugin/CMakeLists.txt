cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(isosurface_plugin VERSION 0.0.1)

set(SOURCE_FILES
	isosurface_plugin.cxx
)

set(HEADER_FILES
	isosurface_plugin.h
	isosurface_plugin.hxx
)

set(SHADER_FILES
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(${PROJECT_NAME} egs triangle_plugin)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION egs/lib/plugins/${PROJECT_NAME})
install(FILES ${SHADER_FILES} DESTINATION egs/lib/plugins/${PROJECT_NAME})
install(FILES ${HEADER_FILES} DESTINATION egs/include)
