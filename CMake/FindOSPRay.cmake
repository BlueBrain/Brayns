## ======================================================================================= ##
## Copyright 2014-2015 Texas Advanced Computing Center, The University of Texas at Austin  ##
## All rights reserved.                                                                    ##
##                                                                                         ##
## Licensed under the BSD 3-Clause License, (the "License"); you may not use this file     ##
## except in compliance with the License.                                                  ##
## A copy of the License is included with this software in the file LICENSE.               ##
## If your copy does not contain the License, you may obtain a copy of the License at:     ##
##                                                                                         ##
##     http://opensource.org/licenses/BSD-3-Clause                                         ##
##                                                                                         ##
## Unless required by applicable law or agreed to in writing, software distributed under   ##
## the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY ##
## KIND, either express or implied.                                                        ##
## See the License for the specific language governing permissions and limitations under   ##
## limitations under the License.                                                          ##
## ======================================================================================= ##


###############################################################################
# Find OSPRay
# defines:
# OSPRAY_FOUND
# OSPRAY_INCLUDE_DIRS
# OSPRAY_LIBRARIES

set(OSPRAY_ROOT $ENV{OSPRAY_ROOT})

# guess that OSPRay is installed in a peer directory (if in dev) or in a peer to the ParaView source
find_path(OSPRAY_ROOT ospray
  HINTS ${PROJECT_SOURCE_DIR}/../OSPRay  ${PROJECT_SOURCE_DIR}/../../../OSPRay
  DOC "OSPRay base directory"
  )

if(NOT OSPRAY_ROOT AND NOT OSPRay_FIND_QUIETLY)
  message("Could not find OSPRay base directory. Please set OSPRAY_ROOT to the root of your local OSPRay git repository.")
endif()

find_path(OSPRAY_ROOT/bin ospModelViewer
  HINTS ${OSPRAY_ROOT}/build ${PROJECT_SOURCE_DIR}/../OSPRay/build ${PROJECT_SOURCE_DIR}/../OSPRay ${PROJECT_SOURCE_DIR}/../../../OSPRay/build ${PROJECT_SOURCE_DIR}/../../../OSPRay
  DOC "OSPRay build directory"
  )
if(NOT OSPRAY_ROOT AND NOT OSPRay_FIND_QUIETLY)
  message("Could not find OSPRay build directory. Please set OSPRAY_ROOT to the directory where OSPRay was built.")
endif()

set(OSPRAY_INCLUDE_DIRS
  ${OSPRAY_ROOT}/include
  ${OSPRAY_ROOT}/include/ospray
  ${OSPRAY_ROOT}/include/ospray/embree/common
  ${OSPRAY_ROOT}/include/ospray/embree
  ${OSPRAY_ROOT}/include/ospray/embree/include
  ${OSPRAY_ROOT}/include/ospray/include
  )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OSPRAY_CMAKE_DIR} ${OSPRAY_DIR})
# which compiler was used to build OSPRay
set(OSPRAY_CC ${OSP_OSPRAY_COMPILER} CACHE STRING "OSPRay Compiler (ICC, GCC, CLANG)")
# whehter to build in MIC/xeon phi support
set(OSPRAY_MIC ${OSP_OSPRAY_BUILD_MIC_SUPPORT} CACHE BOOL "Was OSPRay buit with Xeon Phi Support?")
# whehter to build in MIC/xeon phi support
set(OSPRAY_MPI ${OSP_OSPRAY_BUILD_MPI_DEVICE} CACHE BOOL "Was OSPRay built with MPI Remote/Distributed rendering support?")

add_definitions(${OSPRAY_EMBREE_CXX_FLAGS})

if(OSPRAY_CMAKE_DIR)
  include(${OSPRAY_CMAKE_DIR}/ospray.cmake)
  include(${OSPRAY_CMAKE_DIR}/mpi.cmake)
endif()

if(APPLE)
  set(LIB_EMBREE LIB_EMBREE-NOTFOUND)
  find_library(LIB_EMBREE embree.2.7.1 ${OSPRAY_ROOT}/lib ${OSPRAY_ROOT}/lib/x86_64-linux-gnu)
endif()

set(LIB_OSPRAY LIB_OSPRAY-NOTFOUND)
find_library(LIB_OSPRAY ospray ${OSPRAY_ROOT}/lib ${OSPRAY_ROOT}/lib/x86_64-linux-gnu)
if(OSPRAY_MIC)
  # Xeon Phi specific build ops here
endif()

set(OSPRAY_LIBRARIES ${LIB_OSPRAY} ${LIB_EMBREE})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OSPRay DEFAULT_MSG
                                  OSPRAY_LIBRARIES
                                  OSPRAY_INCLUDE_DIRS)
