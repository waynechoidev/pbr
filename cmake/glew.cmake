add_definitions(-DGLEW_STATIC)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glew)

set(GLEW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glew)
set(GLEW_LIBRARIES ${GLEW_LIBRARIES} libglew_static)