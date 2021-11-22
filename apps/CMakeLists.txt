# BRAYNS applications

if(BRAYNS_VIEWER_ENABLED OR BRAYNS_IBL_ENABLED)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 QUIET)
endif()

add_subdirectory(deps)

if(BRAYNS_VIEWER_ENABLED)
  add_subdirectory(ui)
  add_subdirectory(BraynsViewer)
endif()

if(BRAYNS_IBL_ENABLED)
  add_subdirectory(BraynsIBL)
endif()

if(BRAYNS_SERVICE_ENABLED)
  add_subdirectory(BraynsService)
endif()

if(BRAYNS_BENCHMARK_ENABLED)
  add_subdirectory(BraynsBenchmark)
endif()
