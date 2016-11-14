# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Raphael Dumusc <raphael.dumusc@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(_cmake_stringify_ptx ${CMAKE_CURRENT_LIST_DIR}/StringifyPtx.cmake)

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
        -P ${_cmake_stringify_ptx}
        DEPENDS ${_input}
      )

    list(APPEND _output_headers ${_output_header})
    list(APPEND _output_sources ${_output_source})
  endforeach()

  set(PTX_HEADERS ${_output_headers} PARENT_SCOPE)
  set(PTX_SOURCES ${_output_sources} PARENT_SCOPE)
endfunction()

if(STRINGIFY_PTX_PROCESSING_MODE)
  get_filename_component(_filename ${INPUT} NAME)
  string(REGEX REPLACE "[.]" "_" _name ${_filename})

  file(STRINGS ${INPUT} _lines)

  file(WRITE ${OUTPUT}.h
    "/* Generated file, do not edit! */\n\n"
    "extern char const* const ${_name};\n"
  )

  file(WRITE ${OUTPUT}.cpp
    "/* Generated file, do not edit! */\n\n"
    "#include \"${_filename}.h\"\n\n"
    "char const* const ${_name} = \n"
  )

  foreach(_line ${_lines})
    string(REPLACE "\"" "\\\"" _line "${_line}")
    file(APPEND ${OUTPUT}.cpp "   \"${_line}\\n\"\n")
  endforeach()

  file(APPEND ${OUTPUT}.cpp "   ;\n")
endif()
