# Copyright 2015-2024 Blue Brain Project/EPFL
# Responsible Authors: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSTASKS_SOURCES
  AddModelFromBlobTask.cpp
  AddModelTask.cpp
  LoadModelFunctor.cpp
)

set(BRAYNSTASKS_PUBLIC_HEADERS
  AddModelFromBlobTask.h
  AddModelTask.h
  LoadModelFunctor.h
  SnapshotTask.h
  Errors.h
)

add_library(braynsTasks SHARED ${BRAYNSTASKS_SOURCES} ${BRAYNSTASKS_PUBLIC_HEADERS})

target_include_directories(braynsTasks PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(braynsTasks PUBLIC braynsCommon braynsEngine braynsIO)

target_compile_options(braynsTasks PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsTasks
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsTasks
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
