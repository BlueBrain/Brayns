## ======================================================================== ##
## Copyright 2009-2015 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##
##
## Adaptation:
##    - Raphael Dumusc <raphael.dumusc@epfl.ch>, 2015 BlueBrain project
##
## Defines macro ispc_compile() for compiling sources with the ispc compiler
##
## Input (required):
##   * ISPC_TARGET_LIST a list of target architectures (sse4 avx avx2)
## Input (optional):
##   * ISPC_BINARY path to an ispc executable (default: use ispc in system PATH)
##   * ISPC_DEFINES Additional definitions for the compiler
##   * ISPC_INCLUDE_DIR Include directory for the compiler
##   * THIS_IS_MIC (remove?)
##   * OSPRAY_BUILD_ISA (remove?)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

MACRO(ispc_compile)
  IF(CMAKE_OSX_ARCHITECTURES STREQUAL "i386")
    SET(ISPC_ARCHITECTURE "x86")
  ELSE()
    SET(ISPC_ARCHITECTURE "x86-64")
  ENDIF()

  IF(ISPC_INCLUDE_DIR)
    STRING(REGEX REPLACE ";" ";-I;" ISPC_INCLUDE_DIR_PARMS "${ISPC_INCLUDE_DIR}")
    SET(ISPC_INCLUDE_DIR_PARMS "-I" ${ISPC_INCLUDE_DIR_PARMS})
  ENDIF()

  IF(THIS_IS_MIC)
    SET(CMAKE_ISPC_FLAGS --opt=force-aligned-memory --target generic-16 --emit-c++ --c++-include-file=${PROJECT_SOURCE_DIR}/ospray/common/ISPC_KNC_Backend.h  --addressing=32)
  ELSE()
    SET(CMAKE_ISPC_TARGET "")
    SET(COMMA "")
    FOREACH(target ${ISPC_TARGET_LIST})
      SET(CMAKE_ISPC_TARGET "${CMAKE_ISPC_TARGET}${COMMA}${target}")
      SET(COMMA ",")
    ENDFOREACH()
    SET(CMAKE_ISPC_FLAGS --target=${CMAKE_ISPC_TARGET} --addressing=32)
  ENDIF()

  IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    SET(CMAKE_ISPC_OPT_FLAGS -O3)
  ELSE()
    SET(CMAKE_ISPC_OPT_FLAGS -O2 -g)
  ENDIF()

  SET(ISPC_OBJECTS "")
  FOREACH(src ${ARGN})
    SET(ISPC_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR})

    GET_FILENAME_COMPONENT(fname ${src} NAME_WE)
    GET_FILENAME_COMPONENT(dir ${src} PATH)

    IF("${dir}" MATCHES "^/")
      # ------------------------------------------------------------------
      # global path name to input, like when we include the embree sources
      # from a global external embree checkout
      # ------------------------------------------------------------------
      STRING(REGEX REPLACE "^/" "${CMAKE_CURRENT_BINARY_DIR}/rebased/" outdir "${dir}")
      SET(indir "${dir}")
      SET(input "${indir}/${fname}.ispc")
    ELSE()
      # ------------------------------------------------------------------
      # local path name to input, like local ospray sources
      # ------------------------------------------------------------------
      SET(outdir "${CMAKE_CURRENT_BINARY_DIR}/local_${dir}")
      SET(indir "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
      SET(input "${indir}/${fname}.ispc")
    ENDIF()
    SET(outdirh ${ISPC_TARGET_DIR})

    SET(deps "")
    IF(EXISTS ${outdir}/${fname}.dev.idep)
      FILE(READ ${outdir}/${fname}.dev.idep contents)
      STRING(REGEX REPLACE " " ";"     contents "${contents}")
      STRING(REGEX REPLACE ";" "\\\\;" contents "${contents}")
      STRING(REGEX REPLACE "\n" ";"    contents "${contents}")
      FOREACH(dep ${contents})
        IF (EXISTS ${dep})
          SET(deps ${deps} ${dep})
        ENDIF (EXISTS ${dep})
      ENDFOREACH(dep ${contents})
    ENDIF()

    LIST(LENGTH ISPC_TARGET_LIST numIspcTargets)
    IF(THIS_IS_MIC)
      SET(outputs ${outdir}/${fname}.dev.cpp ${outdirh}/${fname}_ispc.h)
      SET(main_output ${outdir}/${fname}.dev.cpp)
      SET(ISPC_OBJECTS ${ISPC_OBJECTS} ${outdir}/${fname}.dev.cpp)
    ELSEIF("${OSPRAY_BUILD_ISA}" STREQUAL "AVX512")
      SET(outputs ${outdir}/${fname}.dev.cpp ${outdirh}/${fname}_ispc.h)
      SET(ISPC_OBJECTS ${ISPC_OBJECTS} ${outdir}/${fname}.dev.cpp)
      SET(main_output ${outdir}/${fname}.dev.cpp)
      SET(CMAKE_ISPC_FLAGS --target generic-16 --emit-c++ --c++-include-file=${PROJECT_SOURCE_DIR}/ospray/common/ISPC_KNL_Backend.h  --addressing=32)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -xMIC-AVX512")
    ELSEIF(${numIspcTargets} EQUAL 1)
      SET(outputs ${outdir}/${fname}.dev.o ${outdirh}/${fname}_ispc.h)
      SET(ISPC_OBJECTS ${ISPC_OBJECTS} ${outdir}/${fname}.dev.o)
      SET(main_output ${outdir}/${fname}.dev.o)
    ELSE()
      SET(outputs ${outdir}/${fname}.dev.o ${outdirh}/${fname}_ispc.h)
      SET(ISPC_OBJECTS ${ISPC_OBJECTS} ${outdir}/${fname}.dev.o)
      SET(main_output ${outdir}/${fname}.dev.o)
      FOREACH(target ${ISPC_TARGET_LIST})
        SET(outputs ${outputs} ${outdir}/${fname}.dev_${target}.o)
        SET(ISPC_OBJECTS ${ISPC_OBJECTS} ${outdir}/${fname}.dev_${target}.o)
      ENDFOREACH()
    ENDIF()

    if(NOT ISPC_BINARY)
      set(ISPC_BINARY ispc) # Use the ispc executable in path
    endif()

    ADD_CUSTOM_COMMAND(
      OUTPUT ${outputs}
      COMMAND mkdir -p ${outdir} \; ${ISPC_BINARY}
      -I ${CMAKE_CURRENT_SOURCE_DIR}
      ${ISPC_INCLUDE_DIR_PARMS}
      ${ISPC_DEFINES}
      --arch=${ISPC_ARCHITECTURE}
      --pic
      ${CMAKE_ISPC_OPT_FLAGS}
      --woff
      ${CMAKE_ISPC_FLAGS}
      --opt=fast-math
      -h ${outdirh}/${fname}_ispc.h
      -MMM  ${outdir}/${fname}.dev.idep
      -o ${main_output}
      ${input}
      \;
      DEPENDS ${input}
      ${deps})

  ENDFOREACH()

ENDMACRO()
