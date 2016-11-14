# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Raphael Dumusc <raphael.dumusc@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(CMAKE_STRINGIFY_PTX ${CMAKE_CURRENT_LIST_DIR})

function(stringify_ptx)
  set(_output_headers)
  set(_output_sources)
  foreach(_file ${ARGV})
    set(_input ${CMAKE_CURRENT_BINARY_DIR}/${_file})
    set(_output ${CMAKE_CURRENT_BINARY_DIR}/${_file})
    set(_output_header ${_output}.h)
    set(_output_source ${_output}.cpp)

    add_custom_command(OUTPUT ${_output_header} ${_output_source}
      COMMAND ${CMAKE_COMMAND} -DSTRINGIFY_PTX_PROCESSING_MODE=ON
        -DINPUT="${_input}" -DOUTPUT="${_output}"
        -P ${CMAKE_STRINGIFY_PTX}/StringifyPtx.cmake
        DEPENDS ${_input}
      )

    list(APPEND _output_headers ${_output_header})
    list(APPEND _output_sources ${_output_source})
  endforeach(_file ${ARGN})

  set(PTX_HEADERS ${_output_headers} PARENT_SCOPE)
  set(PTX_SOURCES ${_output_sources} PARENT_SCOPE)
endfunction()

if(STRINGIFY_PTX_PROCESSING_MODE)
  get_filename_component(FILENAME ${INPUT} NAME)
  string(REGEX REPLACE "[.]" "_" NAME ${FILENAME})

  file(STRINGS ${INPUT} LINES)

  file(WRITE ${OUTPUT}.h
    "/* Generated file, do not edit! */\n\n"
    "extern char const* const ${NAME};\n"
    )

  file(WRITE ${OUTPUT}.cpp
    "/* Generated file, do not edit! */\n\n"
    "#include \"${FILENAME}.h\"\n\n"
    "char const* const ${NAME} = \n"
    )

  foreach(LINE ${LINES})
    string(REPLACE "\"" "\\\"" LINE "${LINE}")
    file(APPEND ${OUTPUT}.cpp "   \"${LINE}\\n\"\n")
  endforeach(LINE)

  file(APPEND ${OUTPUT}.cpp "   ;\n")
endif()
