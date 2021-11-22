# FROM https://github.com/skui-org/cmake/blob/master/FindLibJpegTurbo.cmake
#
# - Try to find libjpeg-turbo
# Once done, this will define
#
#  LibJpegTurbo_FOUND - system has libjpeg-turbo
#  LibJpegTurbo_INCLUDE_DIRS - the libjpeg-turbo include directories
#  LibJpegTurbo_LIBRARIES - link these to use libjpeg-turbo
#  LibJpegTurbo_VERSION - the version of libjpeg-turbo
#
# this file is modeled after http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(LibJpegTurbo_PKGCONF LibJpegTurbo)

# Include dir
find_path(LibJpegTurbo_INCLUDE_DIR
  NAMES turbojpeg.h
  PATHS ${LibJpegTurbo_PKGCONF_INCLUDE_DIRS} /opt/libjpeg-turbo/include $ENV{LIBJPEGTURBO_ROOT}/include
)

# Search for header with version: jconfig.h
if(LibJpegTurbo_INCLUDE_DIR)
  if(EXISTS "${LibJpegTurbo_INCLUDE_DIR}/jconfig.h")
    set(_version_header "${LibJpegTurbo_INCLUDE_DIR}/jconfig.h")
  elseif(EXISTS "${LibJpegTurbo_INCLUDE_DIR}/x86_64-linux-gnu/jconfig.h")
    set(_version_header "${LibJpegTurbo_INCLUDE_DIR}/x86_64-linux-gnu/jconfig.h")
  else()
    set(_version_header)
    if(NOT LibJpegTurbo_FIND_QUIETLY)
      message(STATUS "Could not find 'jconfig.h' to check version")
    endif()
  endif()
endif()

# Found the header, read version
if(_version_header)
  file(READ "${_version_header}" _header)
  if(_header)
   string(REGEX REPLACE ".*#define[\t ]+LIBJPEG_TURBO_VERSION[\t ]+([0-9.]+).*"
     "\\1" LibJpegTurbo_VERSION "${_header}")
  endif()
  unset(_header)
endif()

# Finally the library itself
find_library(LibJpegTurbo_LIBRARY
  NAMES libturbojpeg.so libturbojpeg.so.0 turbojpeg.lib libturbojpeg.dylib
  PATHS ${LibJpegTurbo_PKGCONF_LIBRARY_DIRS} /opt/libjpeg-turbo/lib $ENV{LIBJPEGTURBO_ROOT}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibJpegTurbo FOUND_VAR LibJpegTurbo_FOUND
                                  REQUIRED_VARS LibJpegTurbo_LIBRARY
                                  LibJpegTurbo_INCLUDE_DIR LibJpegTurbo_VERSION
                                  VERSION_VAR LibJpegTurbo_VERSION)

if(LibJpegTurbo_FOUND)
  set(LibJpegTurbo_INCLUDE_DIRS ${LibJpegTurbo_INCLUDE_DIR})
  set(LibJpegTurbo_LIBRARIES ${LibJpegTurbo_LIBRARY})
  if(NOT LibJpegTurbo_FIND_QUIETLY)
    message(STATUS "Found LibJpegTurbo in ${LibJpegTurbo_INCLUDE_DIR}:${LibJpegTurbo_LIBRARIES}")
  endif()
else()
  set(LibJpegTurbo_INCLUDE_DIR)
  set(LibJpegTurbo_INCLUDE_DIRS)
  set(LibJpegTurbo_LIBRARY)
  set(LibJpegTurbo_LIBRARIES)
  set(LibJpegTurbo_VERSION)
endif()
