# Copyright (c) 2015-2021, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSNETWORK_HEADERS
  common/FrameExporter.h
  entrypoint/Entrypoint.h
  entrypoint/EntrypointManager.h
  entrypoint/IEntrypoint.h
  entrypoint/EntrypointException.h
  entrypoints/ExportFramesEntrypoint.h
  entrypoints/ExportFramesToDiskEntryPoint.h
  entrypoints/GetExportFramesProgressEntrypoint.h
  entrypoints/SnapshotEntrypoint.h
  interface/ActionInterface.h
  json/MessageFactory.h
  plugin/NetworkManager.h
)

set(BRAYNSNETWORK_SOURCES
  binary/BinaryRequestManager.cpp
  binary/ModelUploadTask.cpp
  common/ExtractMaterial.cpp
  common/ExtractModel.cpp
  common/FrameExporter.cpp
  common/RateLimiter.cpp
  context/NetworkContext.cpp
  entrypoint/BaseEntrypoint.cpp
  entrypoint/EntrypointException.cpp
  entrypoint/EntrypointRef.cpp
  entrypoint/EntrypointManager.cpp
  entrypoints/AddClipPlaneEntrypoint.cpp
  entrypoints/AddLightEntrypoint.cpp
  entrypoints/AddModelEntrypoint.cpp
  entrypoints/AnimationParametersEntrypoint.cpp
  entrypoints/ApplicationParametersEntrypoint.cpp
  entrypoints/CameraEntrypoint.cpp
  entrypoints/CameraParamsEntrypoint.cpp
  entrypoints/CancelEntrypoint.cpp
  entrypoints/ChunkEntrypoint.cpp
  entrypoints/ClearLightsEntrypoint.cpp
  entrypoints/ExportFramesEntrypoint.cpp
  entrypoints/ExportFramesToDiskEntryPoint.cpp
  entrypoints/GetExportFramesProgressEntrypoint.cpp
  entrypoints/SnapshotEntrypoint.cpp
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
