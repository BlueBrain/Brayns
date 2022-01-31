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
  adapters/ModelTransferFunctionAdapter.cpp
  adapters/UpdateModelAdapter.cpp
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
  entrypoints/ExitLaterEntrypoint.cpp
  entrypoints/ExportFramesEntrypoint.cpp
  entrypoints/ExportFramesToDiskEntryPoint.cpp
  entrypoints/GetClipPlanesEntrypoint.cpp
  entrypoints/GetExportFramesProgressEntrypoint.cpp
  entrypoints/GetInstancesEntrypoint.cpp
  entrypoints/GetLightsEntrypoint.cpp
  entrypoints/GetLoadersEntrypoint.cpp
  entrypoints/GetModelEntrypoint.cpp
  entrypoints/ImageJpegEntrypoint.cpp
  entrypoints/ImageStreamingModeEntrypoint.cpp
  entrypoints/InspectEntrypoint.cpp
  entrypoints/LoadersSchemaEntrypoint.cpp
  entrypoints/ModelPropertiesEntrypoint.cpp
  entrypoints/ModelTransferFunctionEntrypoint.cpp
  entrypoints/QuitEntrypoint.cpp
  entrypoints/RegistryEntrypoint.cpp
  entrypoints/RemoveClipPlanesEntrypoint.cpp
  entrypoints/RemoveLightsEntrypoint.cpp
  entrypoints/RemoveModelEntrypoint.cpp
  entrypoints/RendererEntrypoint.cpp
  entrypoints/RendererParamsEntrypoint.cpp
  entrypoints/RequestModelUploadEntrypoint.cpp
  entrypoints/ResetCameraEntrypoint.cpp
  entrypoints/SceneEntrypoint.cpp
  entrypoints/SchemaEntrypoint.cpp
  entrypoints/SnapshotEntrypoint.cpp
  entrypoints/StatisticsEntrypoint.cpp
  entrypoints/TriggerJpegStreamEntrypoint.cpp
  entrypoints/UpdateClipPlaneEntrypoint.cpp
  entrypoints/UpdateInstanceEntrypoint.cpp
  entrypoints/UpdateModelEntrypoint.cpp
  entrypoints/VersionEntrypoint.cpp
  entrypoints/VolumeParametersEntrypoint.cpp
  interface/NetworkInterface.cpp
  interface/ClientInterface.cpp
  interface/ServerInterface.cpp
  json/MessageFactory.cpp
  json/PropertyObjectSchema.cpp
  json/RequestId.cpp
  messages/LightMessage.cpp
  plugin/NetworkManager.cpp
  socket/ConnectionHandle.cpp
  socket/ConnectionManager.cpp
  socket/ConnectionMap.cpp
  socket/ConnectionRef.cpp
  socket/NetworkRequest.cpp
  socket/NetworkSocket.cpp
  stream/StreamManager.cpp
  tasks/NetworkTask.cpp
  tasks/NetworkTaskManager.cpp
  tasks/NetworkTaskMonitor.cpp
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
