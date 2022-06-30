# Install the module with the rest of the libraries of Brayns
include(GetPythonLibPath)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/brayns DESTINATION ${PYTHON_LIBRARY_SUFFIX})
