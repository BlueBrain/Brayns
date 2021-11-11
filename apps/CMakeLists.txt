# BRAYNS applications
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
