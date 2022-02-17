# BRAYNS applications

if(BRAYNS_VIEWER_ENABLED)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 QUIET)
endif()

add_subdirectory(deps)

if(BRAYNS_VIEWER_ENABLED)
  add_subdirectory(BraynsViewer)
endif()

if(BRAYNS_SERVICE_ENABLED)
  add_subdirectory(BraynsService)
endif()
