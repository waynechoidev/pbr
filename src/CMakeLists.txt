make_executable()

include_directories(
    ${GLEW_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}
    )
    
target_link_libraries(${PROJECT}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${IMGUI_LIBRARIES}
    )
