cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(egs VERSION 0.0.1)

if (EGS_DEBUG)
	add_definitions(-DDEBUG=1)
	add_definitions(-g)
endif()

set(SOURCE_FILES
	c_wrapper_dle.cxx
  camera_movement_mixin.cxx
	compile_shader.cxx
	context.cxx
  gl_interactivity_mixin.cxx
	gl_context.cxx
	gl_offscreen_context.cxx
	glfw_context.cxx
	glip_glfw_context.cxx
	glip_glfw_protocol_impl.c
	displaylist.cxx
	light.cxx
	propertystore.cxx
  glip_event_list.c
)

set(HEADER_FILES
    c_api.h
	c_wrapper_dle.hxx
  camera_movement_mixin.hxx
	compile_shader.hxx
	context.hxx
	gl_context.hxx
	gl_offscreen_context.hxx
	glfw_context.hxx
	glip_glfw_context.hxx
	glip_glfw_protocol_impl.h
	displaylist.hxx
	light.h
  glip_event_list.h
	propertystore.hxx
  egs.h
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} glip ${PYTHON_LIBRARIES} ${OPENGL_LIBRARIES} png zlib glfw)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${PYTHON_INCLUDE_DIRS})

install(TARGETS ${PROJECT_NAME} DESTINATION egs/lib)
install(FILES ${HEADER_FILES} DESTINATION egs/include)

set(SOURCE_FILES
	glip_glfw_server.c
  glip_event_list.c
)

set(HEADER_FILES
	glip_glfw_server.h
  glip_event_list.h
)

add_executable(glip_glfw_server ${SOURCE_FILES})
target_link_libraries(glip_glfw_server ${PROJECT_NAME} util)

install(TARGETS glip_glfw_server DESTINATION egs/bin)

