cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(glip VERSION 0.0.1)

if (GLIP_DEBUG)
	add_definitions(-DGLIP_DEBUG=1)
endif()

set(SOURCE_FILES
	glad.c
	glip.c
	glip_impl.c
)

set(HEADER_FILES
	glad.h
	glip.h
	glip_impl.h
	khrplatform.h
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} zeromq)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../../thirdparty/build/prefix/include/)

install(TARGETS ${PROJECT_NAME} DESTINATION egs/lib)
install(FILES ${HEADER_FILES} DESTINATION egs/include/glip)
