# Install the module with the rest of the libraries of Brayns

execute_process(COMMAND
  ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1,0,prefix=''))"
  OUTPUT_VARIABLE PYTHON_LIBRARY_SUFFIX OUTPUT_STRIP_TRAILING_WHITESPACE)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/brayns
        DESTINATION ${PYTHON_LIBRARY_SUFFIX}/brion)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/brayns DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Build the module to upload to PyPi
file(COPY ${CMAKE_CURRENT_LIST_DIR} DESTINATION ${CMAKE_BINARY_DIR}/python_client)

add_custom_target(
    brayns_python_client_package
    ALL
    COMMAND ${Python3_EXECUTABLE} setup.py sdist
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/python_client/python
    COMMENT "Building brayns python client")



