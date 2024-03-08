add_subdirectory(${CMAKE_SOURCE_DIR}/external/glm)

set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glm)
set(GLM_LIBRARIES ${GLM_LIBRARIES} glm)