# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSNETWORK_HEADERS
  entrypoint/Entrypoint.h
  entrypoint/EntrypointManager.h
  entrypoint/IEntrypoint.h
  entrypoint/EntrypointException.h
  interface/ActionInterface.h
  json/MessageFactory.h
  plugin/NetworkManager.h
)

set(BRAYNSNETWORK_SOURCES
  entrypoint/EntrypointManager.cpp
  interface/NetworkInterface.cpp
  interface/ClientInterface.cpp
  interface/ServerInterface.cpp
  plugin/NetworkManager.cpp
  socket/ConnectionManager.cpp
  stream/StreamManager.cpp
)

set(BRAYNSNETWORK_INCLUDE_DIR ${PROJECT_SOURCE_DIR})

add_library(braynsNetwork SHARED ${BRAYNSNETWORK_SOURCES} ${BRAYNSNETWORK_HEADERS})

target_include_directories(braynsNetwork PUBLIC
  $<BUILD_INTERFACE:${BRAYNSNETWORK_INCLUDE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(braynsNetwork PRIVATE
  braynsCommon
  braynsParameters
  braynsPluginAPI
  braynsUtils
)

target_link_libraries(braynsNetwork PUBLIC
  braynsJson
  braynsTasks
  braynsIO
  Poco::Foundation
  Poco::Net
  Poco::NetSSL
)

target_compile_options(braynsNetwork PRIVATE ${BRAYNS_COMPILE_OPTIONS})

set_target_properties(braynsNetwork
    PROPERTIES
        VERSION ${BRAYNS_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS braynsNetwork
    EXPORT Brayns-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
