## ======================================================================== ##
## Copyright 2009-2016 Intel Corporation                                    ##
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

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-strict-aliasing  -std=c++11 -Wno-narrowing")
SET(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG  -g     -Wstrict-aliasing=1")
SET(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG    -O3 -Wstrict-aliasing=1 -ffast-math ")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DNDEBUG -g -O3 -Wstrict-aliasing=1 -ffast-math ")

IF (APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7")
ENDIF (APPLE)

# these flags apply ONLY to how embree is built; the rest of the ospray C++ code is ISA-agnostic
SET(OSPRAY_ARCH_SSE3    "-msse3")
SET(OSPRAY_ARCH_SSSE3   "-mssse3")
SET(OSPRAY_ARCH_SSE41   "-msse4.1")
SET(OSPRAY_ARCH_SSE42   "-msse4.2")
SET(OSPRAY_ARCH_AVX     "-mavx")
SET(OSPRAY_ARCH_AVX2   "-mf16c -mavx2 -mfma -mlzcnt -mbmi -mbmi2")
SET(OSPRAY_ARCH_AVX512 "-mavx512f -mavx512pf -mavx512er -mavx512cd")


# check whether ICC version is new enough for C++11
SET(ICC_VERSION_REQUIRED "15.0.0")

IF(NOT ICC_VERSION)
  EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE ICC_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE)
  SET(ICC_VERSION ${ICC_OUTPUT} CACHE STRING "ICC Version")
  MARK_AS_ADVANCED(ICC_VERSION)
ENDIF()

IF (ICC_VERSION VERSION_LESS ICC_VERSION_REQUIRED)
  MESSAGE(FATAL_ERROR "ICC version ${ICC_VERSION_REQUIRED} or greater is required to build OSPRay.")
ENDIF()

# ICC 4.8.0 supports AVX and AVX2...
SET(OSPRAY_COMPILER_SUPPORTS_AVX TRUE)
SET(OSPRAY_COMPILER_SUPPORTS_AVX2 TRUE)

SET(ICC_VERSION_REQUIRED_AVX512 "15.0.0")

IF (ICC_VERSION VERSION_LESS ICC_VERSION_REQUIRED_AVX512)
  SET(OSPRAY_COMPILER_SUPPORTS_AVX512 FALSE)
ELSE()
  SET(OSPRAY_COMPILER_SUPPORTS_AVX512 TRUE)
ENDIF()
