cmake_minimum_required(VERSION 3.5.0)
project(opengl-template VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(opengl-template
    src/main.cpp
    src/window.h
    src/window.cpp
)

# glfw
find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(external/glew EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)

# add_custom_command(
#         TARGET ${PROJECT_NAME} PRE_BUILD
#         COMMAND ${CMAKE_COMMAND} -E create_symlink
#                 ${CMAKE_SOURCE_DIR}/Shaders
#                 ${CMAKE_CURRENT_BINARY_DIR}/Shaders)

# add_custom_command(
#         TARGET ${PROJECT_NAME} PRE_BUILD
#         COMMAND ${CMAKE_COMMAND} -E create_symlink
#                 ${CMAKE_SOURCE_DIR}/Textures
#                 ${CMAKE_CURRENT_BINARY_DIR}/Textures)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)